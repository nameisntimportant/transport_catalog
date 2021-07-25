#pragma once

#include "baseRequests.h"
#include "graph.h"
#include "json.h"
#include "routeDistancesDict.h"
#include "router.h"

#include "makeVisitor.h"
#include "profiler.h"

#include <memory>
#include <unordered_map>
#include <vector>

class TransportRouter
{
public:
    struct RouteElement
    {
        int waitTime;
        std::string bus;
        std::string from;
        size_t spanCount;
        double transitTime;
    };

    struct RouteStats
    {
        double totalTime;
        std::vector<RouteElement> routeElements;
    };

private:
    using RoutesGraph = Graph::DirectedWeightedGraph<double>;
    using RoutesGraphPtr = std::unique_ptr<RoutesGraph>;
    using Router = Graph::Router<double>;
    using RouterPtr = std::unique_ptr<Router>;

    struct RoutingSettings
    {
        int busWaitTime = 0;
        double busVelocity = 0;
    };

public:
    TransportRouter(const BaseRequests::ParsedBuses& buses,
                    const RouteDistancesMap& routeDistances,
                    const Json::Map& routingSettings);

    std::optional<RouteStats> findRoute(const std::string& from, const std::string& to) const;

private:
    void createGraph(const BaseRequests::ParsedBuses& buses);
    void fillGraphWithEdges(const BaseRequests::ParsedBuses& buses,
                            const RouteDistancesMap& routeDistances,
                            const RoutingSettings& routingSettings);

    static RoutingSettings makeRoutingSettings(const Json::Map& routingSettingsMap);

private:
    RoutesGraphPtr graph_;
    RouterPtr router_;

    std::unordered_map<std::string, Graph::VertexId> stopToVertex_;
    std::unordered_map<Graph::EdgeId, RouteElement> edgeToRouteElement_;

    RoutingSettings routingSettings_;
};
