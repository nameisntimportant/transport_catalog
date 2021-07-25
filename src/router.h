#pragma once

#include "utils.h"
#include "graph.h"
#include "profiler.h"

#include <algorithm>

namespace Graph
{
template <typename Weight>
class Router
{
private:
    using Graph = DirectedWeightedGraph<Weight>;
    using ExpandedRoute = std::vector<EdgeId>;

    struct RouteInternalData
    {
        Weight weight;
        std::optional<EdgeId> prevEdge;
    };
    using RoutesInternalData = std::vector<std::vector<std::optional<RouteInternalData>>>;

public:
    using RouteId = uint64_t;

    struct RouteInfo
    {
        RouteId id;
        Weight weight;
        size_t edgeCount;
    };

    Router(const Graph& graph);

    std::optional<RouteInfo> buildRoute(VertexId from, VertexId to) const;
    EdgeId getRouteEdge(RouteId routeId, size_t edgeIndex) const;
    void releaseRoute(RouteId routeId);

private:
    void initializeRoutesInternalData();

    void relaxRoute(VertexId vertexFrom,
                    VertexId vertexTo,
                    const RouteInternalData& routeFrom,
                    const RouteInternalData& routeTo);

    void relaxRoutesInternalDataThroughVertex(size_t vertexCount, VertexId vertexThrough);

private:
    const Graph& graph_;
    mutable RouteId nextRouteId_ = 0;

    mutable std::unordered_map<RouteId, ExpandedRoute> expandedRoutesCache_;
    RoutesInternalData routesInternalData_;
};

template <typename Weight>
Router<Weight>::Router(const Graph& graph)
    : graph_(graph)
    , routesInternalData_(graph.getVertexCount(),
                          std::vector<std::optional<RouteInternalData>>(graph.getVertexCount()))
{
    LOG_DURATION("Router ctor");

    initializeRoutesInternalData();

    const size_t vertexCount = graph.getVertexCount();
    for (VertexId vertexThrough = 0; vertexThrough < vertexCount; ++vertexThrough)
    {
        relaxRoutesInternalDataThroughVertex(vertexCount, vertexThrough);
    }
}

template <typename Weight>
void Router<Weight>::initializeRoutesInternalData()
{
    const size_t vertexCount = graph_.getVertexCount();
    for (VertexId vertex = 0; vertex < vertexCount; ++vertex)
    {
        routesInternalData_[vertex][vertex] = RouteInternalData{0, std::nullopt};
        for (const EdgeId edgeId : graph_.getEdgesWhichStartFrom(vertex))
        {
            const auto& edge = graph_.getEdge(edgeId);
            ASSERT_WITH_MESSAGE(edge.weight >= 0, "Router works only with edges with non-negative weight");

            auto& routeInternalData = routesInternalData_[vertex][edge.to];
            if (!routeInternalData || routeInternalData->weight > edge.weight)
            {
                routeInternalData = RouteInternalData{edge.weight, edgeId};
            }
        }
    }
}

template <typename Weight>
void Router<Weight>::relaxRoute(VertexId vertexFrom,
                                VertexId vertexTo,
                                const RouteInternalData& routeFrom,
                                const RouteInternalData& routeTo)
{
    auto& routeRelaxing = routesInternalData_[vertexFrom][vertexTo];
    const Weight candidateWeight = routeFrom.weight + routeTo.weight;
    if (!routeRelaxing || candidateWeight < routeRelaxing->weight)
    {
        routeRelaxing = {candidateWeight,
                         routeTo.prevEdge ? routeTo.prevEdge : routeFrom.prevEdge};
    }
}

template <typename Weight>
void Router<Weight>::relaxRoutesInternalDataThroughVertex(size_t vertexCount,
                                                          VertexId vertexThrough)
{
    for (VertexId vertexFrom = 0; vertexFrom < vertexCount; ++vertexFrom)
    {
        if (const auto& routeFrom = routesInternalData_[vertexFrom][vertexThrough])
        {
            for (VertexId vertexTo = 0; vertexTo < vertexCount; ++vertexTo)
            {
                if (const auto& routeTo = routesInternalData_[vertexThrough][vertexTo])
                {
                    relaxRoute(vertexFrom, vertexTo, *routeFrom, *routeTo);
                }
            }
        }
    }
}

template <typename Weight>
std::optional<typename Router<Weight>::RouteInfo> Router<Weight>::buildRoute(VertexId from,
                                                                             VertexId to) const
{
    const auto& routeInternalData = routesInternalData_[from][to];
    if (!routeInternalData)
    {
        return std::nullopt;
    }

    std::vector<EdgeId> edges;
    for (std::optional<EdgeId> edgeId = routeInternalData->prevEdge; edgeId;
         edgeId = routesInternalData_[from][graph_.getEdge(*edgeId).from]->prevEdge)
    {
        edges.push_back(*edgeId);
    }
    std::reverse(std::begin(edges), std::end(edges));

    const RouteId routeId = nextRouteId_++;
    const size_t routeEdgeCount = edges.size();
    const Weight weight = routeInternalData->weight;

    expandedRoutesCache_[routeId] = std::move(edges);

    return RouteInfo{routeId, weight, routeEdgeCount};
}

template <typename Weight>
EdgeId Router<Weight>::getRouteEdge(RouteId routeId, size_t edgeIndex) const
{
    return expandedRoutesCache_.at(routeId)[edgeIndex];
}

template <typename Weight>
void Router<Weight>::releaseRoute(RouteId routeId)
{
    expandedRoutesCache_.erase(routeId);
}

inline bool operator==(const typename Router<double>::RouteInfo& lhs, const typename Router<double>::RouteInfo& rhs)
{
    return lhs.id == rhs.id && fuzzyCompare(lhs.weight, rhs.weight) && lhs.edgeCount == rhs.edgeCount;
}

inline std::ostream& operator<<(std::ostream& stream, const std::optional<typename Router<double>::RouteInfo>& routeInfoOpt)
{
    if (!routeInfoOpt)
    {
        return stream << "route not found";
    }
    return stream << "id " << routeInfoOpt->id << " weight " << routeInfoOpt->weight << " edgeCount " << routeInfoOpt->edgeCount;
}

} // namespace Graph
