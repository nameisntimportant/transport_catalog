#pragma once

#include "utils.h"

#include <type_traits>
#include <vector>

namespace Graph
{

using VertexId = size_t;
using EdgeId = size_t;

template <typename Weight>
struct Edge
{
    VertexId from;
    VertexId to;
    Weight weight;
};

template <typename Weight>
bool operator==(const Edge<Weight>& lhs, const Edge<Weight>& rhs)
{
    return lhs.from == rhs.from && lhs.to == rhs.to && lhs.weight == rhs.weight;
}

template <typename Weight>
std::ostream& operator<<(std::ostream& stream, const Edge<Weight>& edge)
{
    return stream << "from: " << edge.from << " to: " << edge.to << " weight: " << edge.weight;
}

template <typename Weight>
class DirectedWeightedGraph
{
private:
    using EdgesVector = std::vector<EdgeId>;
    using VertexesRange = Range<typename EdgesVector::const_iterator>;

public:
    DirectedWeightedGraph(size_t vertexСount = 0);
    EdgeId addEdge(const Edge<Weight>& edge);

    size_t getVertexCount() const;
    size_t getEdgeCount() const;
    const Edge<Weight>& getEdge(EdgeId edgeId) const;
    VertexesRange edgesWhichStartFrom(VertexId vertex) const;

private:
    std::vector<Edge<Weight>> edges_;
    std::vector<EdgesVector> vertexes_;
};

template <typename Weight>
DirectedWeightedGraph<Weight>::DirectedWeightedGraph(size_t vertexCount)
    : vertexes_(vertexCount)
{
}

template <typename Weight>
EdgeId DirectedWeightedGraph<Weight>::addEdge(const Edge<Weight>& edge)
{
    edges_.push_back(edge);
    const EdgeId id = edges_.size() - 1;
    vertexes_[edge.from].push_back(id);
    return id;
}

template <typename Weight>
size_t DirectedWeightedGraph<Weight>::getVertexCount() const
{
    return vertexes_.size();
}

template <typename Weight>
size_t DirectedWeightedGraph<Weight>::getEdgeCount() const
{
    return edges_.size();
}

template <typename Weight>
const Edge<Weight>& DirectedWeightedGraph<Weight>::getEdge(EdgeId edgeId) const
{
    return edges_[edgeId];
}

template <typename Weight>
typename DirectedWeightedGraph<Weight>::VertexesRange DirectedWeightedGraph<
    Weight>::edgesWhichStartFrom(VertexId vertex) const
{
    const auto& edges = vertexes_[vertex];
    return {std::begin(edges), std::end(edges)};
}

} // namespace Graph
