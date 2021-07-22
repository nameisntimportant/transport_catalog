#pragma once

#include "baseRequests.h"
#include "graph.h"
#include "json.h"
#include "routeDistancesDict.h"
#include "router.h"

#include "makeVisitor.h"
#include "profiler.h"

#include "tests/transportRouterTestSuite.h"

#include <memory>
#include <unordered_map>
#include <vector>

class TransportRouter
{
public:
    struct BusRouteElement
    {
        std::string bus;
        size_t spanCount;
        double time;
    };

    struct WaitRouteElement
    {
        double time;
        std::string stopName;
    };

    using RouteElement = std::variant<WaitRouteElement, BusRouteElement>;

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

    using NameAndTime = std::pair<std::string, double>;

    struct RouterData
    {
        std::unordered_map<std::string, Graph::VertexId> fromUniqueStopToVertexId;
        std::unordered_map<std::string, Graph::VertexId> fromNonUniqueStopToTransferVertexId;
        std::unordered_map<Graph::EdgeId, NameAndTime> fromEdgeToNameAndTime;
        std::unordered_set<Graph::EdgeId> transferEdges;
    };

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
    static std::tuple<RoutesGraphPtr, RouterData> buildRouteGraph(
        const BaseRequests::ParsedBuses& buses,
        const RouteDistancesMap& routeDistances,
        const RoutingSettings& routingSettings);

    Graph::VertexId getVertexId(const std::string& stopName) const;
    NameAndTime getNameAndTime(Graph::EdgeId edgeId) const;
    bool isTransferEdge(Graph::EdgeId edgeId) const;
    bool containsStop(const std::string &stopName) const;

private:
    RoutesGraphPtr graph_;
    RouterPtr router_;
    RouterData data_;

    std::unordered_map<std::string, Graph::VertexId> stopToVertex_;
    std::unordered_map<Graph::EdgeId, RouteElement> edgeToRouteElement_;

    RoutingSettings routingSettings_;

    friend void Tests::testBuildRouteGraph();
    friend void Tests::testFindRoute();
};

inline bool operator==(const TransportRouter::WaitRouteElement& lhs, const TransportRouter::WaitRouteElement& rhs)
{
    return lhs.stopName == rhs.stopName && fuzzyCompare(lhs.time, rhs.time);
}

inline bool operator==(const TransportRouter::BusRouteElement& lhs, const TransportRouter::BusRouteElement& rhs)
{
    return lhs.bus == rhs.bus && lhs.spanCount == rhs.spanCount && fuzzyCompare(lhs.time, rhs.time);
}

inline bool operator==(const TransportRouter::RouteStats& lhs, const TransportRouter::RouteStats& rhs)
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

inline std::ostream& operator<<(std::ostream& stream, const TransportRouter::RouteStats& lhs)
{
    stream << "total time " << lhs.totalTime;
    stream << " route elements { ";
    for (const auto& item : lhs.routeElements)
    {
        std::visit(make_visitor(
                       [&stream](const TransportRouter::WaitRouteElement& element) {
                           stream << "Wait: stop name " << element.stopName << " time "
                                  << element.time << ".";
                       },
                       [&stream](const TransportRouter::BusRouteElement& element) {
                           stream << "Bus: bus name " << element.bus << " span count " << element.spanCount << " time " << element.time << ".";
                       }),
                   item);
        stream << " ";
    }
    return stream << "}";
}
