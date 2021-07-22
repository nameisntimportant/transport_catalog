#include "../router.h"

#include "testRunner.h"
#include "utils.h"

#include <exception>
#include <vector>

using namespace std;

namespace Graph
{
namespace Tests
{
using RouteInfo = Router<double>::RouteInfo;

namespace
{
template <typename Weight>
void assertEdgesInRouteAreEqualTo(const Router<Weight>& router,
                                  const optional<RouteInfo>& routeInfo,
                                  const vector<Graph::EdgeId>& expectedEdges)
{
    ASSERT(bool(routeInfo));
    vector<Graph::EdgeId> actualEdges;
    for (size_t i = 0; i < routeInfo->edgeCount; i++)
    {
        actualEdges.push_back(router.getRouteEdge(routeInfo->id, i));
    }
    ASSERT_EQUAL(actualEdges, expectedEdges);
}
constexpr auto EmptyRouteInfo = optional<RouteInfo>();
} // namespace

void testBuildNonExistingRoute()
{
    DirectedWeightedGraph<double> graph(2);
    graph.addEdge({1, 0, 23.44});
    Router<double> router(graph);
    ASSERT_EQUAL(router.buildRoute(0, 1), EmptyRouteInfo);
}

void testTwoDifferentEdgesBetweenOnePairOfVertexes()
{
    DirectedWeightedGraph<double> graph(2);
    graph.addEdge({0, 1, 4541222.345});
    graph.addEdge({0, 1, 9442.44});
    Router<double> router(graph);

    constexpr auto expected = RouteInfo({.id = 0, .weight = 9442.44, .edgeCount = 1});
    ASSERT_EQUAL(router.buildRoute(0, 1), expected);
    ASSERT_EQUAL(router.getRouteEdge(expected.id, 0), 1);

    ASSERT_EQUAL(router.buildRoute(1, 0), EmptyRouteInfo);
}

void testBuildRouteFromTheVertexToTheSameVertex()
{
    DirectedWeightedGraph<double> graph(2);
    graph.addEdge({0, 1, 2});
    graph.addEdge({1, 0, 456});
    Router<double> router(graph);
    ASSERT_EQUAL(router.buildRoute(0, 0), RouteInfo({.id = 0, .weight = 0, .edgeCount = 0}));
    ASSERT_EQUAL(router.buildRoute(1, 1), RouteInfo({.id = 1, .weight = 0, .edgeCount = 0}));
}

void testRouteThroughThreeEdgesIsShortedThanThroughOne()
{
    DirectedWeightedGraph<double> graph(4);
    graph.addEdge({0, 1, 70.33254});
    graph.addEdge({2, 1, 31.4545});
    graph.addEdge({3, 2, 14.852});
    graph.addEdge({0, 3, 17.32});
    Router<double> router(graph);

    const auto route = router.buildRoute(0, 1);
    ASSERT_EQUAL(route, RouteInfo({.id = 0, .weight = 63.6265, .edgeCount = 3}));
    assertEdgesInRouteAreEqualTo(router, route, {3, 2, 1});
}

void testCircleGraph()
{
    DirectedWeightedGraph<double> graph(3);
    graph.addEdge({0, 1, 100.17});
    graph.addEdge({1, 2, 77});
    graph.addEdge({2, 0, 12});
    graph.addEdge({2, 1, 114.886});
    Router<double> router(graph);

    {
        auto route = router.buildRoute(2, 1);
        ASSERT_EQUAL(route, RouteInfo({.id = 0, .weight = 112.17, .edgeCount = 2}));
        assertEdgesInRouteAreEqualTo(router, route, {2, 0});
    }

    {
        auto route = router.buildRoute(1, 0);
        ASSERT_EQUAL(route, RouteInfo({.id = 1, .weight = 89, .edgeCount = 2}));
        assertEdgesInRouteAreEqualTo(router, route, {1, 2});
    }
}

void testGraphWithSeveralVetexesAndEdges()
{
    DirectedWeightedGraph<double> graph(8);
    graph.addEdge({0, 1, 114.7});
    graph.addEdge({2, 3, 581.44});
    graph.addEdge({4, 3, 743.88});
    graph.addEdge({4, 5, 965.115});
    graph.addEdge({4, 7, 717.18});
    graph.addEdge({7, 6, 99});
    graph.addEdge({3, 2, 15.48});
    graph.addEdge({5, 6, 1.1});
    graph.addEdge({6, 5, 61.1});
    graph.addEdge({6, 3, 744.5});
    graph.addEdge({3, 6, 0.114});
    graph.addEdge({5, 3, 9.221});
    Router<double> router(graph);

    ASSERT_EQUAL(router.buildRoute(0, 1), RouteInfo({.id = 0, .weight = 114.7, .edgeCount = 1}));
    ASSERT_EQUAL(router.getRouteEdge(0, 0), 0);

    {
        auto route = router.buildRoute(7, 2);
        ASSERT_EQUAL(route, RouteInfo({.id = 1, .weight = 184.801, .edgeCount = 4}));
        assertEdgesInRouteAreEqualTo(router, route, {5, 8, 11, 6});
    }

    {
        auto route = router.buildRoute(6, 2);
        ASSERT_EQUAL(route, RouteInfo({.id = 2, .weight = 85.801, .edgeCount = 3}));
        assertEdgesInRouteAreEqualTo(router, route, {8, 11, 6});
    }

    {
        auto route = router.buildRoute(4, 6);
        ASSERT_EQUAL(route, RouteInfo({.id = 3, .weight = 743.994, .edgeCount = 2}));
        assertEdgesInRouteAreEqualTo(router, route, {2, 10});
    }

    {
        auto route = router.buildRoute(5, 3);
        ASSERT_EQUAL(route, RouteInfo({.id = 4, .weight = 9.221, .edgeCount = 1}));
        assertEdgesInRouteAreEqualTo(router, route, {11});
    }

    {
        auto route = router.buildRoute(6, 3);
        ASSERT_EQUAL(route, RouteInfo({.id = 5, .weight = 70.321, .edgeCount = 2}));
        assertEdgesInRouteAreEqualTo(router, route, {8, 11});
    }

    {
        auto route = router.buildRoute(7, 3);
        ASSERT_EQUAL(route, RouteInfo({.id = 6, .weight = 169.321, .edgeCount = 3}));
        assertEdgesInRouteAreEqualTo(router, route, {5, 8, 11});
    }

    {
        auto route = router.buildRoute(7, 5);
        ASSERT_EQUAL(route, RouteInfo({.id = 7, .weight = 160.1, .edgeCount = 2}));
        assertEdgesInRouteAreEqualTo(router, route, {5, 8});
    }

    {
        auto route = router.buildRoute(2, 5);
        ASSERT_EQUAL(route, RouteInfo({.id = 8, .weight = 642.654, .edgeCount = 3}));
        assertEdgesInRouteAreEqualTo(router, route, {1, 10, 8});
    }

    {
        auto route = router.buildRoute(2, 6);
        ASSERT_EQUAL(route, RouteInfo({.id = 9, .weight = 581.554, .edgeCount = 2}));
        assertEdgesInRouteAreEqualTo(router, route, {1, 10});
    }

    {
        auto route = router.buildRoute(3, 5);
        ASSERT_EQUAL(route, RouteInfo({.id = 10, .weight = 61.214, .edgeCount = 2}));
        assertEdgesInRouteAreEqualTo(router, route, {10, 8});
    }

    {
        auto route = router.buildRoute(4, 2);
        ASSERT_EQUAL(route, RouteInfo({.id = 11, .weight = 759.36, .edgeCount = 2}));
        assertEdgesInRouteAreEqualTo(router, route, {2, 6});
    }

    ASSERT_EQUAL(router.buildRoute(2, 1), EmptyRouteInfo);
    ASSERT_EQUAL(router.buildRoute(3, 1), EmptyRouteInfo);
    ASSERT_EQUAL(router.buildRoute(2, 0), EmptyRouteInfo);
    ASSERT_EQUAL(router.buildRoute(2, 7), EmptyRouteInfo);
}

void testReleaseRoute()
{
    DirectedWeightedGraph<double> graph(2);
    graph.addEdge({0, 1, 9442.44});
    graph.addEdge({1, 0, 4541222.345});
    Router<double> router(graph);

    const auto routeIdFirst = router.buildRoute(0, 1)->id;
    const auto routeIdSecond = router.buildRoute(1, 0)->id;
    ASSERT_EQUAL(router.getRouteEdge(routeIdFirst, 0), 0);
    ASSERT_EQUAL(router.getRouteEdge(routeIdSecond, 0), 1);

    router.releaseRoute(routeIdFirst);
    ASSERT_EXCEPTION_THROWN(router.getRouteEdge(routeIdFirst, 0), out_of_range);

    // check if the first route is still cached
    ASSERT_EQUAL(router.getRouteEdge(routeIdSecond, 0), 1);

    router.releaseRoute(routeIdSecond);
    ASSERT_EXCEPTION_THROWN(router.getRouteEdge(routeIdSecond, 0), out_of_range);

    const auto routeIdAfterReleasingRoutes = router.buildRoute(0, 1)->id;
    ASSERT_EQUAL(routeIdAfterReleasingRoutes, 2);
}

void runRouterTests()
{
    TestRunner tr;
    RUN_TEST(tr, testBuildNonExistingRoute);
    RUN_TEST(tr, testTwoDifferentEdgesBetweenOnePairOfVertexes);
    RUN_TEST(tr, testBuildRouteFromTheVertexToTheSameVertex);
    RUN_TEST(tr, testRouteThroughThreeEdgesIsShortedThanThroughOne);
    RUN_TEST(tr, testCircleGraph);
    RUN_TEST(tr, testGraphWithSeveralVetexesAndEdges);
    RUN_TEST(tr, testReleaseRoute);
};
} // namespace Tests
} // namespace Graph
