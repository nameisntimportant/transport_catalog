#include "transportRouterTestSuite.h"
#include "../transportRouter.h"
#include "testRunner.h"

using namespace std;

using RouteStats = TransportRouter::RouteStats;
using WaitRouteElement = TransportRouter::WaitRouteElement;
using BusRouteElement = TransportRouter::BusRouteElement;

namespace
{
const auto EmptyRouteOptional = optional<RouteStats>();
}
namespace Tests
{

void testBuildRouteGraph()
{
    BaseRequests::ParsedBuses buses{{.name = "simpleRoundTripBus", .stops = {"A", "B", "C", "A"}}};
    RouteDistancesMap routeDistances{{{"A", "B"}, 2}, {{"B", "C"}, 5}, {{"C", "A"}, 11}};
    TransportRouter::RoutingSettings routingSetting{.busWaitTime = 6, .busVelocity = 40.0};

    const auto [simpleCircleBusGraph, graphInfo] =
        TransportRouter::buildRouteGraph(buses, routeDistances, routingSetting);

    ASSERT_EQUAL(simpleCircleBusGraph->getVertexCount(), 5);
    ASSERT_EQUAL(simpleCircleBusGraph->getEdgeCount(), 7);

    // route edges
    ASSERT_EQUAL(simpleCircleBusGraph->getEdge(0),
                 Graph::Edge<double>({0, 1, 2.0 / routingSetting.busVelocity}));
    ASSERT_EQUAL(simpleCircleBusGraph->getEdge(1),
                 Graph::Edge<double>({1, 2, 5.0 / routingSetting.busVelocity}));
    ASSERT_EQUAL(simpleCircleBusGraph->getEdge(2),
                 Graph::Edge<double>({2, 4, 11.0 / routingSetting.busVelocity}));

    // transfer edges
    ASSERT_EQUAL(
        simpleCircleBusGraph->getEdge(3),
        Graph::Edge<double>({0, 3, static_cast<double>(routingSetting.busWaitTime) / 2.0}));
    ASSERT_EQUAL(
        simpleCircleBusGraph->getEdge(4),
        Graph::Edge<double>({3, 0, static_cast<double>(routingSetting.busWaitTime) / 2.0}));
    ASSERT_EQUAL(
        simpleCircleBusGraph->getEdge(5),
        Graph::Edge<double>({4, 3, static_cast<double>(routingSetting.busWaitTime) / 2.0}));
    ASSERT_EQUAL(
        simpleCircleBusGraph->getEdge(6),
        Graph::Edge<double>({3, 4, static_cast<double>(routingSetting.busWaitTime) / 2.0}));

    ASSERT(graphInfo.transferEdges.count(3) != 0);
    ASSERT(graphInfo.transferEdges.count(4) != 0);
    ASSERT(graphInfo.transferEdges.count(5) != 0);
    ASSERT(graphInfo.transferEdges.count(6) != 0);
    ASSERT(graphInfo.transferEdges.count(0) == 0);
    ASSERT(graphInfo.transferEdges.count(1) == 0);
    ASSERT(graphInfo.transferEdges.count(2) == 0);
}

void testFindRoute()
{
    {
        // Test regular cases
        BaseRequests::ParsedBuses buses{{.name = "297",
                                         .stops = {"Biryulyovo Zapadnoye",
                                                   "Biryulyovo Tovarnaya",
                                                   "Universam",
                                                   "Biryulyovo Zapadnoye"}},
                                        {.name = "635",
                                         .stops = {"Biryulyovo Tovarnaya",
                                                   "Universam",
                                                   "Prazhskaya",
                                                   "Universam",
                                                   "Biryulyovo Tovarnaya"}}};

        RouteDistancesMap routeDistances{{{"Biryulyovo Zapadnoye", "Biryulyovo Tovarnaya"}, 2600},
                                         {{"Biryulyovo Tovarnaya", "Biryulyovo Zapadnoye"}, 2600},
                                         {{"Biryulyovo Tovarnaya", "Universam"}, 890},
                                         {{"Universam", "Biryulyovo Tovarnaya"}, 1380},
                                         {{"Universam", "Biryulyovo Zapadnoye"}, 2500},
                                         {{"Biryulyovo Zapadnoye", "Universam"}, 2500},
                                         {{"Universam", "Prazhskaya"}, 4650},
                                         {{"Prazhskaya", "Universam"}, 4650}};

        Json::Map routingSetting{{"bus_wait_time", 6}, {"bus_velocity", 40.0}};

        const auto transportRouter = TransportRouter(buses, routeDistances, routingSetting);

        {
            const auto expectedOneWay = RouteStats{
                .totalTime = 11.235,
                .routeElements = {WaitRouteElement{.time = 6, .stopName = "Biryulyovo Zapadnoye"},
                                  BusRouteElement{.bus = "297", .spanCount = 2, .time = 5.235}}};
            ASSERT_EQUAL(transportRouter.findRoute("Biryulyovo Zapadnoye", "Universam"),
                         expectedOneWay);

            const auto expectedWayBack = RouteStats{
                .totalTime = 11.235,
                .routeElements = {WaitRouteElement{.time = 6, .stopName = "Biryulyovo Zapadnoye"},
                                  BusRouteElement{.bus = "297", .spanCount = 2, .time = 5.235}}};
            ASSERT_EQUAL(transportRouter.findRoute("Biryulyovo Zapadnoye", "Universam"),
                         expectedWayBack);
        }

        {
            const auto expectedOneWay = RouteStats{
                .totalTime = 24.21,
                .routeElements = {WaitRouteElement{.time = 6, .stopName = "Biryulyovo Zapadnoye"},
                                  BusRouteElement{.bus = "297", .spanCount = 1, .time = 3.9},
                                  WaitRouteElement{.time = 6, .stopName = "Biryulyovo Tovarnaya"},
                                  BusRouteElement{.bus = "635", .spanCount = 2, .time = 8.31}}};
            ASSERT_EQUAL(transportRouter.findRoute("Biryulyovo Zapadnoye", "Prazhskaya"),
                         expectedOneWay);

            const auto expectedWayBack = RouteStats{
                .totalTime = 24.21,
                .routeElements = {WaitRouteElement{.time = 6, .stopName = "Biryulyovo Zapadnoye"},
                                  BusRouteElement{.bus = "297", .spanCount = 1, .time = 3.9},
                                  WaitRouteElement{.time = 6, .stopName = "Biryulyovo Tovarnaya"},
                                  BusRouteElement{.bus = "635", .spanCount = 2, .time = 8.31}}};
            ASSERT_EQUAL(transportRouter.findRoute("Biryulyovo Zapadnoye", "Prazhskaya"),
                         expectedWayBack);
        }

        {
            const auto expectedOneWay = RouteStats{
                .totalTime = 7.335,
                .routeElements = {WaitRouteElement{.time = 6, .stopName = "Biryulyovo Tovarnaya"},
                                  BusRouteElement{.bus = "297", .spanCount = 1, .time = 1.335}}};
            ASSERT_EQUAL(transportRouter.findRoute("Biryulyovo Tovarnaya", "Universam"),
                         expectedOneWay);

            const auto expectedWayBack = RouteStats{
                .totalTime = 8.07,
                .routeElements = {WaitRouteElement{.time = 6, .stopName = "Universam"},
                                  BusRouteElement{.bus = "635", .spanCount = 1, .time = 2.07}}};
            ASSERT_EQUAL(transportRouter.findRoute("Universam", "Biryulyovo Tovarnaya"),
                         expectedWayBack);
        }
    }

    {
        // Test special cases
        BaseRequests::ParsedBuses buses{{.name = "289",
                                         .stops = {"Zagorye",
                                                   "Lipetskaya ulitsa 46",
                                                   "Lipetskaya ulitsa 40",
                                                   "Lipetskaya ulitsa 40",
                                                   "Lipetskaya ulitsa 46",
                                                   "Moskvorechye",
                                                   "Zagorye"}}};

        RouteDistancesMap routeDistances{{{"Zagorye", "Lipetskaya ulitsa 46"}, 230},
                                         {{"Lipetskaya ulitsa 46", "Lipetskaya ulitsa 40"}, 390},
                                         {{"Lipetskaya ulitsa 46", "Moskvorechye"}, 12400},
                                         {{"Lipetskaya ulitsa 40", "Lipetskaya ulitsa 40"}, 1090},
                                         {{"Lipetskaya ulitsa 40", "Lipetskaya ulitsa 46"}, 380},
                                         {{"Moskvorechye", "Zagorye"}, 10000}};

        Json::Map routingSetting{{"bus_wait_time", 2}, {"bus_velocity", 48.561}};

        const auto transportRouter = TransportRouter(buses, routeDistances, routingSetting);

        // Get off at the bus stop and transfer to the same bus going in the other direction is
        // the fastest way
        const auto expected = RouteStats{
            .totalTime = 19.6051,
            .routeElements = {WaitRouteElement{.time = 2, .stopName = "Zagorye"},
                              BusRouteElement{.bus = "289", .spanCount = 1, .time = 0.284179},
                              WaitRouteElement{.time = 2, .stopName = "Lipetskaya ulitsa 46"},
                              BusRouteElement{.bus = "289", .spanCount = 1, .time = 15.3209}}};
        ASSERT_EQUAL(transportRouter.findRoute("Zagorye", "Moskvorechye"), expected);

        // Find the route from Lipetskaya ulitsa 40 to itself then there is the route from
        // Lipetskaya ulitsa 40 to itself
        ASSERT_EQUAL(transportRouter.findRoute("Lipetskaya ulitsa 40", "Lipetskaya ulitsa 40"),
                     RouteStats{});

        // Find the route from Biryulyovo Zapadnoye to itself then there is NO route from
        // Biryulyovo Zapadnoye to itself
        ASSERT_EQUAL(transportRouter.findRoute("Biryulyovo Zapadnoye", "Biryulyovo Zapadnoye"),
                     RouteStats{});

        ASSERT_EQUAL(transportRouter.findRoute("nonExistingStop", "Moskvorechye"),
                     EmptyRouteOptional);
        ASSERT_EQUAL(transportRouter.findRoute("Zagorye", "nonExistingStop"), EmptyRouteOptional);
        ASSERT_EQUAL(transportRouter.findRoute("nonExistingStop", "OtherNonExistingStop"),
                     EmptyRouteOptional);
    }
}

void runTransportRouterTests()
{
    TestRunner tr;
    RUN_TEST(tr, testBuildRouteGraph);
    RUN_TEST(tr, testFindRoute);
};
} // namespace Tests
