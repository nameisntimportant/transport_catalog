#include "transportRouter.h"

using namespace std;

namespace
{
constexpr double FromKmPerHourToMPerMinute = 1000.0 / 60.0;
}

TransportRouter::TransportRouter(const BaseRequests::ParsedBuses& buses,
                                 const RouteDistancesMap& routeDistances,
                                 const Json::Map& routingSettingsMap)
{
    createGraph(buses);
    fillGraphWithEdges(buses, routeDistances, makeRoutingSettings(routingSettingsMap));
    router_ = make_unique<Router>(*graph_);
}

void TransportRouter::createGraph(const BaseRequests::ParsedBuses& buses)
{
    Graph::VertexId currentVertexId = 0;
    for (const auto& [busName, goingThroughStops] : buses)
    {
        for (const auto& currentStop : goingThroughStops)
        {
            if (stopToVertex_.count(currentStop) == 0)
            {
                stopToVertex_.emplace(currentStop, currentVertexId);
                currentVertexId++;
            }
        }
    }
    graph_ = make_unique<RoutesGraph>(currentVertexId);
}

void TransportRouter::fillGraphWithEdges(const BaseRequests::ParsedBuses& buses,
                                         const RouteDistancesMap& routeDistances,
                                         const TransportRouter::RoutingSettings& routingSettings)
{
    for (const auto& [busName, goingThroughStops] : buses)
    {
        for (auto departureIt = goingThroughStops.begin(); departureIt < goingThroughStops.end();
             departureIt++)
        {
            size_t summaryDistance = 0;
            for (auto destinationIt = next(departureIt); destinationIt < goingThroughStops.end();
                 destinationIt++)
            {
                if (*departureIt == *destinationIt)
                {
                    continue;
                }

                summaryDistance += routeDistances.at({*prev(destinationIt), *destinationIt});
                const double transitTime = static_cast<double>(summaryDistance) / routingSettings.busVelocity;
                const size_t spanCount = static_cast<size_t>(std::distance(departureIt, destinationIt));
                RouteElement routeElement = {.waitTime = routingSettings.busWaitTime,
                                             .bus = busName,
                                             .from = *departureIt,
                                             .spanCount = spanCount,
                                             .transitTime = transitTime};

                const auto totalTime = transitTime + routingSettings.busWaitTime;
                const auto edgeId = graph_->addEdge(
                    {stopToVertex_.at(*departureIt), stopToVertex_.at(*destinationIt), totalTime});
                edgeToRouteElement_.emplace(edgeId, move(routeElement));
            }
        }
    }
}

TransportRouter::RoutingSettings TransportRouter::makeRoutingSettings(
    const Json::Map& routingSettingsMap)
{
    return {routingSettingsMap.at("bus_wait_time").asInt(),
            routingSettingsMap.at("bus_velocity").asDouble() * FromKmPerHourToMPerMinute};
}

optional<TransportRouter::RouteStats> TransportRouter::findRoute(const string& from,
                                                                 const string& to) const
{
    if (from == to)
    {
        return RouteStats{};
    }

    if (stopToVertex_.count(from) == 0 || stopToVertex_.count(to) == 0)
    {
        return nullopt;
    }

    const auto route = router_->buildRoute(stopToVertex_.at(from), stopToVertex_.at(to));
    if (!route)
    {
        return nullopt;
    }

    RouteStats result;
    result.totalTime = route->weight;

    const auto edgeCount = route->edgeCount;
    for (size_t edgeIndex = 0; edgeIndex < edgeCount; edgeIndex++)
    {
        const auto edgeId = router_->getRouteEdge(route->id, edgeIndex);
        result.routeElements.emplace_back(edgeToRouteElement_.at(edgeId));
    }
    router_->releaseRoute(route->id);

    return result;
}

void TransportRouter::serialize(TCProto::TransportRouter& proto) const
{
    graph_->serialize(*proto.mutable_graph());
    router_->serialize(*proto.mutable_router());

    proto.mutable_vertexes_info()->Reserve(static_cast<int>(stopToVertex_.size()));
    for (const auto& [stopName, vertexId] : stopToVertex_)
    {
        auto& vertexInfoProto = *proto.add_vertexes_info();
        vertexInfoProto.set_stop_name(stopName);
        vertexInfoProto.set_vertex_id(vertexId);
    }

    proto.mutable_edges_info()->Reserve(static_cast<int>(edgeToRouteElement_.size()));
    for (const auto& [edgeId, routeElement] : edgeToRouteElement_)
    {
        auto& edgeInfoProto = *proto.add_edges_info();
        edgeInfoProto.set_edge_id(edgeId);
        edgeInfoProto.set_wait_time(routeElement.waitTime);
        edgeInfoProto.set_bus_name(routeElement.bus);
        edgeInfoProto.set_departure_stop_name(routeElement.from);
        edgeInfoProto.set_span_count(routeElement.spanCount);
        edgeInfoProto.set_transit_time(routeElement.transitTime);
    }
}

unique_ptr<TransportRouter> TransportRouter::deserialize(const TCProto::TransportRouter& proto)
{
    unique_ptr<TransportRouter> transportRouterPtr(
        new TransportRouter); // Ctor is private, so can't use make_unique

    transportRouterPtr->graph_ = make_unique<RoutesGraph>(RoutesGraph::deserialize(proto.graph()));
    transportRouterPtr->router_ = Router::deserialize(proto.router(), *transportRouterPtr->graph_);

    transportRouterPtr->stopToVertex_.reserve(static_cast<size_t>(proto.vertexes_info().size()));
    for (const auto& vertexInfoProto : proto.vertexes_info())
    {
        transportRouterPtr->stopToVertex_[vertexInfoProto.stop_name()] =
            vertexInfoProto.vertex_id();
    }

    transportRouterPtr->edgeToRouteElement_.reserve(static_cast<size_t>(proto.edges_info().size()));
    for (const auto& edgeInfoProto : proto.edges_info())
    {
        transportRouterPtr->edgeToRouteElement_[edgeInfoProto.edge_id()] = {
            .waitTime = edgeInfoProto.wait_time(),
            .bus = edgeInfoProto.bus_name(),
            .from = edgeInfoProto.departure_stop_name(),
            .spanCount = edgeInfoProto.span_count(),
            .transitTime = edgeInfoProto.transit_time()};
    }

    return transportRouterPtr;
}
