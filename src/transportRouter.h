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

// TODO move it to .cpp or separated test project
inline bool operator==(const TransportRouter::RouteElement& lhs,
                       const TransportRouter::RouteElement& rhs)
{
    return lhs.waitTime == rhs.waitTime && lhs.bus == rhs.bus && lhs.from == rhs.from &&
           lhs.spanCount == rhs.spanCount && fuzzyCompare(lhs.transitTime, rhs.transitTime);
}

inline bool operator==(const TransportRouter::RouteStats& lhs,
                       const TransportRouter::RouteStats& rhs)
{
    if (lhs.routeElements.size() != rhs.routeElements.size())
    {
        return false;
    }
    for (size_t i = 0; i < lhs.routeElements.size(); i++)
    {
        if (!(lhs.routeElements[i] == rhs.routeElements[i]))
        {
            return false;
        }
    }
    return fuzzyCompare(lhs.totalTime, rhs.totalTime);
}

inline std::ostream& operator<<(std::ostream& stream, const TransportRouter::RouteStats& stats)
{
    stream << "total time " << stats.totalTime;
    stream << " route elements { ";
    for (const auto& element : stats.routeElements)
    {
        stream << "wait time " << element.waitTime << " bus " << element.bus << " from " << element.from
               << " span count " << element.spanCount << " transit time " << element.transitTime << ". ";
    }
    return stream << "}";
}
