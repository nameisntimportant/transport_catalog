#include "../graph.h"
#include "testRunner.h"
#include "utils.h"

#include <vector>

using namespace std;

namespace Graph
{
namespace Tests
{
void testEmptyGraph()
{
    DirectedWeightedGraph<double> graph;
    ASSERT_EQUAL(graph.getVertexCount(), 0);
    ASSERT_EQUAL(graph.getEdgeCount(), 0);
}

void testDifferentWeightDependingOnDirection()
{
    DirectedWeightedGraph<int> graph(2);
    graph.addEdge({0, 1, -3124});
    graph.addEdge({1, 0, 4951335});

    ASSERT_EQUAL(graph.getEdge(0), Edge<int>({.from = 0, .to = 1, .weight = -3124}));
    ASSERT_EQUAL(graph.getEdge(1), Edge<int>({.from = 1, .to = 0, .weight = 4951335}));

    ASSERT_EQUAL(graph.edgesWhichStartFrom(0), asRange(vector<EdgeId>{0}));
    ASSERT_EQUAL(graph.edgesWhichStartFrom(1), asRange(vector<EdgeId>{1}));

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

    ASSERT_EQUAL(graph.edgesWhichStartFrom(0), asRange(vector<EdgeId>{4}));
    ASSERT_EQUAL(graph.edgesWhichStartFrom(1), asRange(vector<EdgeId>{3}));
    ASSERT_EQUAL(graph.edgesWhichStartFrom(2), asRange(vector<EdgeId>{2}));
    ASSERT_EQUAL(graph.edgesWhichStartFrom(3), asRange(vector<EdgeId>{0, 5}));
    ASSERT_EQUAL(graph.edgesWhichStartFrom(4), asRange(vector<EdgeId>{1}));
    ASSERT_EQUAL(graph.edgesWhichStartFrom(5), asRange(vector<EdgeId>{}));

    ASSERT_EQUAL(graph.getVertexCount(), 6);
    ASSERT_EQUAL(graph.getEdgeCount(), 6);
}

void run()
{
    TestRunner tr;
    RUN_TEST(tr, testEmptyGraph);
    RUN_TEST(tr, testDifferentWeightDependingOnDirection);
    RUN_TEST(tr, testGraphWithSeveralVertexesAndEdges);
};
} // namespace Tests
} // namespace Graph
