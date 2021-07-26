#include "transportRouter.h"
#include "log.h"

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
            double summaryDistance = 0;
            for (auto destinationIt = next(departureIt); destinationIt < goingThroughStops.end();
                 destinationIt++)
            {
                if (*departureIt == *destinationIt)
                {
                    continue;
                }

                summaryDistance += routeDistances.at({*prev(destinationIt), *destinationIt});
                const double transitTime = summaryDistance / routingSettings.busVelocity;
                const size_t spanCount = std::distance(departureIt, destinationIt);
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
