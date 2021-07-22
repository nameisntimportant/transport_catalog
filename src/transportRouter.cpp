#include "transportRouter.h"
#include "log.h"

using namespace std;

namespace
{
constexpr double FromKmPerHourToMPerMinute = 1000.0 / 60.0;
}

TransportRouter::TransportRouter(const BaseRequests::ParsedBuses& buses,
                                 const RouteDistancesMap& routeDistances,
                                 const Json::Map& routingSettings)
{
    LOG_DURATION("TransportRouter ctor");
    routingSettings_ = {routingSettings.at("bus_wait_time").asInt(),
                        routingSettings.at("bus_velocity").asDouble() * FromKmPerHourToMPerMinute};

    tie(graph_, data_) = buildRouteGraph(buses, routeDistances, routingSettings_);

    router_ = make_unique<Router>(*graph_);
}

std::pair<std::string, double> TransportRouter::getNameAndTime(Graph::EdgeId edgeId) const
{
    return data_.fromEdgeToNameAndTime.at(edgeId);
}

bool TransportRouter::containsStop(const std::string& stopName) const
{
    return data_.fromNonUniqueStopToTransferVertexId.count(stopName) != 0 ||
           data_.fromUniqueStopToVertexId.count(stopName) != 0;
}

Graph::VertexId TransportRouter::getVertexId(const std::string& stopName) const
{
    const bool gotTransfer = data_.fromNonUniqueStopToTransferVertexId.count(stopName) != 0;
    return gotTransfer ? data_.fromNonUniqueStopToTransferVertexId.at(stopName)
                       : data_.fromUniqueStopToVertexId.at(stopName);
}

bool TransportRouter::isTransferEdge(Graph::EdgeId edgeId) const
{
    return data_.transferEdges.count(edgeId) != 0;
}

tuple<TransportRouter::RoutesGraphPtr, TransportRouter::RouterData> TransportRouter::
    buildRouteGraph(const BaseRequests::ParsedBuses& busInputInfos,
                    const RouteDistancesMap& routeDistances,
                    const RoutingSettings& routingSettings)
{
    LOG_DURATION("buildingRouteGraph");

    using fromVertexIdToStopName = map<Graph::VertexId, string>;
    unordered_map<string, fromVertexIdToStopName> fromBusNameAndVertexIdToStopName;
    unordered_map<string, Graph::VertexId> fromUniqueStopToVertexId;
    unordered_map<string, Graph::VertexId> fromStopNameToTransferVertexId;

    // Each bus got its own vertexex for stops. So VertexId for stop "X" in bus "A" != VertexId for
    // stop "X" in bus "B". To perfrom transfer between equal stops in defferent beses, we create
    // transfer vertexes and edges.
    Graph::VertexId currentVertexId = 0;
    for (const auto& [busName, goingThroughStops] : busInputInfos)
    {
        for (const auto& currentStop : goingThroughStops)
        {
            // If stop meets several times, create transfer vertex
            if (fromUniqueStopToVertexId.count(currentStop) != 0 &&
                fromStopNameToTransferVertexId.count(currentStop) == 0)
            {
                fromStopNameToTransferVertexId.emplace(currentStop, currentVertexId);
                currentVertexId++;
            }
            else
            {
                fromUniqueStopToVertexId.emplace(currentStop, currentVertexId);
            }

            fromBusNameAndVertexIdToStopName[busName].emplace(currentVertexId, currentStop);

            currentVertexId++;
        }
    }

    auto graph = make_unique<RoutesGraph>(currentVertexId);

    unordered_map<Graph::EdgeId, pair<string, double>> fromEdgeToNameAndTime;
    unordered_set<Graph::EdgeId> transferEdges;

    for (const auto& [busName, fromVertexIdToStop] : fromBusNameAndVertexIdToStopName)
    {
        if (fromVertexIdToStop.size() == 0)
        {
            continue;
        }

        // connect stops in current bus
        for (auto it = fromVertexIdToStop.begin(); next(it) != fromVertexIdToStop.end(); it++)
        {
            const auto& currentVertexId = it->first;
            const auto& currentStopName = it->second;
            const auto& nextVertexId = next(it)->first;
            const auto& nextStopName = next(it)->second;

            const auto routeDistance = routeDistances.at({currentStopName, nextStopName});
            const auto transitTime =
                static_cast<double>(routeDistance) / routingSettings.busVelocity;
            const auto edgeId = graph->addEdge({currentVertexId, nextVertexId, transitTime});

            fromEdgeToNameAndTime.emplace(edgeId, pair{busName, transitTime});
        }

        // create edges between stop vertex and transfer vertex and back
        for (const auto& [stopVertexId, stopName] : fromVertexIdToStop)
        {
            if (fromStopNameToTransferVertexId.count(stopName) != 0)
            {
                const auto halfOfTranfserTime = routingSettings.busWaitTime / 2.0;

                const auto oneWayTranferEdgeId =
                    graph->addEdge({stopVertexId,
                                    fromStopNameToTransferVertexId.at(stopName),
                                    halfOfTranfserTime});
                const auto wayBackTranferEdgeId =
                    graph->addEdge({fromStopNameToTransferVertexId.at(stopName),
                                    stopVertexId,
                                    halfOfTranfserTime});

                transferEdges.emplace(oneWayTranferEdgeId);
                transferEdges.emplace(wayBackTranferEdgeId);

                fromEdgeToNameAndTime.emplace(oneWayTranferEdgeId,
                                              pair{stopName, halfOfTranfserTime});
                fromEdgeToNameAndTime.emplace(wayBackTranferEdgeId,
                                              pair{stopName, halfOfTranfserTime});
            }
        }
    }

    return {move(graph),
            RouterData{move(fromUniqueStopToVertexId),
                       move(fromStopNameToTransferVertexId),
                       move(fromEdgeToNameAndTime),
                       move(transferEdges)}};
}

optional<TransportRouter::RouteStats> TransportRouter::findRoute(const string& from,
                                                                 const string& to) const
{
    LOG_DURATION("finding route");

    if (from == to)
    {
        return RouteStats{};
    }

    if (!containsStop(from) || !(containsStop(to)))
    {
        return nullopt;
    }

    const auto route = router_->buildRoute(getVertexId(from), getVertexId(to));
    if (!route)
    {
        return nullopt;
    }

    RouteStats result;
    result.totalTime = route->weight;

    if (route->edgeCount == 0)
    {
        return result;
    }

    // Waiting on the first stop in the route
    result.totalTime += routingSettings_.busWaitTime;
    result.routeElements.push_back(WaitRouteElement{
        .time = static_cast<double>(routingSettings_.busWaitTime), .stopName = from});

    const auto edgeCount = route->edgeCount;
    for (size_t edgeIndex = 0; edgeIndex < edgeCount; edgeIndex++)
    {
        const auto edgeId = router_->getRouteEdge(route->id, edgeIndex);
        const auto [name, time] = getNameAndTime(edgeId);

        // If route starts or stops with transfer, skip this edge
        if ((edgeIndex == 0 || edgeIndex + 1 == edgeCount) && isTransferEdge(edgeId))
        {
            result.totalTime -= time;
            continue;
        }

        RouteElement element;
        if (isTransferEdge(edgeId))
        {
            element = WaitRouteElement{.time = time * 2, .stopName = std::move(name)};
            // Transfer always contains two edges - from stop to transfer and from transfer to stop.
            // So skip next edge
            edgeIndex++;
        }
        else
        {
            double totalTime = 0;
            unsigned int spanCount = 0;
            while (true)
            {
                if (edgeIndex == edgeCount)
                {
                    edgeIndex--;
                    break;
                }

                const auto edgeId = router_->getRouteEdge(route->id, edgeIndex);

                if (isTransferEdge(edgeId))
                {
                    edgeIndex--;
                    break;
                }

                totalTime += getNameAndTime(edgeId).second;
                spanCount++;

                edgeIndex++;
            };
            element = BusRouteElement{.bus = name, .spanCount = spanCount, .time = totalTime};
        }
        result.routeElements.push_back(element);
    }
    router_->releaseRoute(route->id);

    return result;
}
