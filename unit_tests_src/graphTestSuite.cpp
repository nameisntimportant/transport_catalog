#include "graph.h"
#include "graphTestSuite.h"
#include "testRunner.h"
#include "utils.h"

using namespace std;

namespace
{
template <typename Weight>
bool operator==(const Graph::Edge<Weight>& lhs, const Graph::Edge<Weight>& rhs)
{
    return lhs.from == rhs.from && lhs.to == rhs.to && fuzzyCompare(lhs.weight, rhs.weight);
}

template <typename Weight>
ostream& operator<<(ostream& stream, const Graph::Edge<Weight>& edge)
{
    return stream << "from: " << edge.from << " to: " << edge.to << " weight: " << edge.weight;
}
}

namespace Graph
{
namespace Tests
{
void testEmptyGraph()
{
    DirectedWeightedGraph<double> graph;
    ASSERT_EQUAL(graph.getVertexCount(), 0u);
    ASSERT_EQUAL(graph.getEdgeCount(), 0u);
}

void testDifferentWeightDependingOnDirection()
{
    DirectedWeightedGraph<int> graph(2);
    graph.addEdge({.from = 0, .to = 1, .weight = -3124});
    graph.addEdge({.from = 1, .to = 0, .weight = 4951335});

    ASSERT_EQUAL(graph.getEdge(0), Edge<int>({.from = 0, .to = 1, .weight = -3124}));
    ASSERT_EQUAL(graph.getEdge(1), Edge<int>({.from = 1, .to = 0, .weight = 4951335}));

    ASSERT_EQUAL(graph.getEdgesWhichStartFrom(0), asRange(vector<EdgeId>{0}));
    ASSERT_EQUAL(graph.getEdgesWhichStartFrom(1), asRange(vector<EdgeId>{1}));

    ASSERT_EQUAL(graph.getVertexCount(), 2);
    ASSERT_EQUAL(graph.getEdgeCount(), 2);
}

void testGraphWithSeveralVertexesAndEdges()
{
    DirectedWeightedGraph<float> graph(6);
    graph.addEdge({3, 4, 941.334});
    graph.addEdge({4, 3, -6453.465});
    graph.addEdge({2, 1, 354.2257});
    graph.addEdge({1, 0, -8.5});
    graph.addEdge({0, 3, 6.8465});
    graph.addEdge({3, 0, 6.2873});

    ASSERT_EQUAL(graph.getEdge(0), Edge<float>({.from = 3, .to = 4, .weight = 941.334}));
    ASSERT_EQUAL(graph.getEdge(1), Edge<float>({.from = 4, .to = 3, .weight = -6453.465}));
    ASSERT_EQUAL(graph.getEdge(2), Edge<float>({.from = 2, .to = 1, .weight = 354.2257}));
    ASSERT_EQUAL(graph.getEdge(3), Edge<float>({.from = 1, .to = 0, .weight = -8.5}));
    ASSERT_EQUAL(graph.getEdge(4), Edge<float>({.from = 0, .to = 3, .weight = 6.8465}));
    ASSERT_EQUAL(graph.getEdge(5), Edge<float>({.from = 3, .to = 0, .weight = 6.2873}));

    ASSERT_EQUAL(graph.getEdgesWhichStartFrom(0), asRange(vector<EdgeId>{4}));
    ASSERT_EQUAL(graph.getEdgesWhichStartFrom(1), asRange(vector<EdgeId>{3}));
    ASSERT_EQUAL(graph.getEdgesWhichStartFrom(2), asRange(vector<EdgeId>{2}));
    ASSERT_EQUAL(graph.getEdgesWhichStartFrom(3), asRange(vector<EdgeId>{0, 5}));
    ASSERT_EQUAL(graph.getEdgesWhichStartFrom(4), asRange(vector<EdgeId>{1}));
    ASSERT_EQUAL(graph.getEdgesWhichStartFrom(5), asRange(vector<EdgeId>{}));

    ASSERT_EQUAL(graph.getVertexCount(), 6u);
    ASSERT_EQUAL(graph.getEdgeCount(), 6u);
}

void runGraphTests()
{
    TestRunner tr;
    RUN_TEST(tr, testEmptyGraph);
    RUN_TEST(tr, testDifferentWeightDependingOnDirection);
    RUN_TEST(tr, testGraphWithSeveralVertexesAndEdges);
}
} // namespace Tests
} // namespace Graph
