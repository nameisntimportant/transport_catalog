#include "transportRouterTestSuite.h"
#include "testRunner.h"
#include "transportRouter.h"

using namespace std;

using RouteStats = TransportRouter::RouteStats;
using RouteElement = TransportRouter::RouteElement;

namespace
{
const auto EmptyRouteOptional = optional<RouteStats>();
}

bool operator==(const TransportRouter::RouteElement& lhs, const TransportRouter::RouteElement& rhs)
{
    return lhs.waitTime == rhs.waitTime && lhs.bus == rhs.bus && lhs.from == rhs.from &&
           lhs.spanCount == rhs.spanCount && fuzzyCompare(lhs.transitTime, rhs.transitTime);
}

bool operator==(const TransportRouter::RouteStats& lhs, const TransportRouter::RouteStats& rhs)
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

ostream& operator<<(ostream& stream, const TransportRouter::RouteStats& stats)
{
    stream << "total time " << stats.totalTime;
    stream << " route elements { ";
    for (const auto& element : stats.routeElements)
    {
        stream << "wait time " << element.waitTime << " bus " << element.bus << " from "
               << element.from << " span count " << element.spanCount << " transit time "
               << element.transitTime << ". ";
    }
    return stream << "}";
}

// TODO: split into two functions
namespace Tests
{
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
            const auto expectedOneWay =
                RouteStats{.totalTime = 11.235,
                           .routeElements = {{.waitTime = 6,
                                              .bus = "297",
                                              .from = "Biryulyovo Zapadnoye",
                                              .spanCount = 2,
                                              .transitTime = 5.235}}};
            ASSERT_EQUAL(transportRouter.findRoute("Biryulyovo Zapadnoye", "Universam"),
                         expectedOneWay);

            const auto expectedWayBack =
                RouteStats{.totalTime = 11.235,
                           .routeElements = {{.waitTime = 6,
                                              .bus = "297",
                                              .from = "Biryulyovo Zapadnoye",
                                              .spanCount = 2,
                                              .transitTime = 5.235}}};
            ASSERT_EQUAL(transportRouter.findRoute("Biryulyovo Zapadnoye", "Universam"),
                         expectedWayBack);
        }

        {
            const auto expectedOneWay =
                RouteStats{.totalTime = 24.21,
                           .routeElements = {{.waitTime = 6,
                                              .bus = "297",
                                              .from = "Biryulyovo Zapadnoye",
                                              .spanCount = 1,
                                              .transitTime = 3.9},
                                             {.waitTime = 6,
                                              .bus = "635",
                                              .from = "Biryulyovo Tovarnaya",
                                              .spanCount = 2,
                                              .transitTime = 8.31}}};
            ASSERT_EQUAL(transportRouter.findRoute("Biryulyovo Zapadnoye", "Prazhskaya"),
                         expectedOneWay);

            const auto expectedWayBack =
                RouteStats{.totalTime = 24.21,
                           .routeElements = {{.waitTime = 6,
                                              .bus = "297",
                                              .from = "Biryulyovo Zapadnoye",
                                              .spanCount = 1,
                                              .transitTime = 3.9},
                                             {.waitTime = 6,
                                              .bus = "635",
                                              .from = "Biryulyovo Tovarnaya",
                                              .spanCount = 2,
                                              .transitTime = 8.31}}};
            ASSERT_EQUAL(transportRouter.findRoute("Biryulyovo Zapadnoye", "Prazhskaya"),
                         expectedWayBack);
        }

        {
            const auto expectedOneWay =
                RouteStats{.totalTime = 7.335,
                           .routeElements = {{.waitTime = 6,
                                              .bus = "297",
                                              .from = "Biryulyovo Tovarnaya",
                                              .spanCount = 1,
                                              .transitTime = 1.335}}};
            ASSERT_EQUAL(transportRouter.findRoute("Biryulyovo Tovarnaya", "Universam"),
                         expectedOneWay);

            const auto expectedWayBack = RouteStats{.totalTime = 8.07,
                                                    .routeElements = {{.waitTime = 6,
                                                                       .bus = "635",
                                                                       .from = "Universam",
                                                                       .spanCount = 1,
                                                                       .transitTime = 2.07}}};
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
        const auto expected = RouteStats{.totalTime = 19.6051,
                                         .routeElements = {{.waitTime = 2,
                                                            .bus = "289",
                                                            .from = "Zagorye",
                                                            .spanCount = 1,
                                                            .transitTime = 0.284179},
                                                           {.waitTime = 2,
                                                            .bus = "289",
                                                            .from = "Lipetskaya ulitsa 46",
                                                            .spanCount = 1,
                                                            .transitTime = 15.3209}}};
        ASSERT_EQUAL(transportRouter.findRoute("Zagorye", "Moskvorechye"), expected);

        // Find the route from Lipetskaya ulitsa 40 to itself then there is the route from
        // Lipetskaya ulitsa 40 to itself
        ASSERT_EQUAL(transportRouter.findRoute("Lipetskaya ulitsa 40", "Lipetskaya ulitsa 40"),
                     RouteStats{});

        // Find the route from Biryulyovo Zapadnoye to itself then there is NO route from
        // Biryulyovo Zapadnoye to itself
        ASSERT_EQUAL(transportRouter.findRoute("Biryulyovo Zapadnoye", "Biryulyovo Zapadnoye"),
                     RouteStats{});

        ASSERT_EQUAL(transportRouter.findRoute("NonExistingStop", "Moskvorechye"),
                     EmptyRouteOptional);
        ASSERT_EQUAL(transportRouter.findRoute("Zagorye", "nonExistingStop"), EmptyRouteOptional);
        ASSERT_EQUAL(transportRouter.findRoute("NonExistingStop", "OtherNonExistingStop"),
                     EmptyRouteOptional);
    }
}

void runTransportRouterTests()
{
    TestRunner tr;
    RUN_TEST(tr, testFindRoute);
}
} // namespace Tests
