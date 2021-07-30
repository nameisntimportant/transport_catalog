#pragma once

#include "utils.h"

#include "graph.pb.h"

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
    VertexesRange getEdgesWhichStartFrom(VertexId vertex) const;

    void serialize(GraphProto::DirectedWeightedGraph& proto) const;
    static DirectedWeightedGraph deserialize(const GraphProto::DirectedWeightedGraph& proto);

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
    Weight>::getEdgesWhichStartFrom(VertexId vertex) const
{
    const auto& edges = vertexes_[vertex];
    return {std::begin(edges), std::end(edges)};
}

template <typename Weight>
void DirectedWeightedGraph<Weight>::serialize(GraphProto::DirectedWeightedGraph& proto) const
{
    static_assert(std::is_same_v<Weight, double>,
                  "Serialization is implemented only for double weights");

    for (const auto& edge : edges_)
    {
        auto& edgeProto = *proto.add_edges();
        edgeProto.set_from(edge.from);
        edgeProto.set_to(edge.to);
        edgeProto.set_weight(edge.weight);
    }

    for (const auto& vertex : vertexes_)
    {
        auto& vertexesVectorProto = *proto.add_incidence_lists();
        for (const auto edgeId : vertex)
        {
            vertexesVectorProto.add_edge_ids(edgeId);
        }
    }
}

template <typename Weight>
DirectedWeightedGraph<Weight> DirectedWeightedGraph<Weight>::deserialize(
    const GraphProto::DirectedWeightedGraph& proto)
{
    static_assert(std::is_same_v<Weight, double>,
                  "Serialization is implemented only for double weights");

    DirectedWeightedGraph graph;

    graph.edges_.reserve(proto.edges_size());
    for (const auto& edgeProto : proto.edges())
    {
        auto& edge = graph.edges_.emplace_back();
        edge.from = edgeProto.from();
        edge.to = edgeProto.to();
        edge.weight = edgeProto.weight();
    }

    graph.vertexes_.reserve(proto.incidence_lists_size());
    for (const auto& vertexesVectorProto : proto.incidence_lists())
    {
        auto& vertexes = graph.vertexes_.emplace_back();
        vertexes.reserve(vertexesVectorProto.edge_ids_size());
        for (const auto edgeId : vertexesVectorProto.edge_ids())
        {
            vertexes.push_back(edgeId);
        }
    }

    return graph;
}

} // namespace Graph
