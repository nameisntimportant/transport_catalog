#include "makeBaseRequestsTestSuit.h"
#include "../json.h"
#include "../makeBaseRequests.h"
#include "testRunner.h"

using namespace std;

namespace MakeBaseRequests
{
namespace Tests
{

void testBusParsing()
{
    {
        const auto input = Json::Map{{"name", "NonRoundtripBusABC"},
                                     {"stops", Json::Array{"stopA", "stopB", "stopC"}},
                                     {"is_roundtrip", false}};

        const Bus expected = {.name = "NonRoundtripBusABC", .stops = {"stopA", "stopB", "stopC", "stopB", "stopA"}};

        ASSERT_EQUAL(Bus::parseFrom(input), expected);
    }

    {
        const auto input = Json::Map{{"name", "RoundtripBusCDE"},
                                     {"stops", Json::Array{"stopC", "stopD", "stopE"}},
                                     {"is_roundtrip", true}};

        const Bus expected = {.name = "RoundtripBusCDE", .stops = {"stopC", "stopD", "stopE"}};

        ASSERT_EQUAL(Bus::parseFrom(input), expected);
    }

    {
        ASSERT_EQUAL(Bus::parseFrom(Json::Map{{"name", "NoStopsBus"}, {"stops", Json::Array{}}}),
                     Bus({.name = "NoStopsBus", .stops = {}}));
    }
}

void testStopParsing()
{
    {
        const Json::Map roadDistancesJson = {
            {"anotherNeighbourStop", 57}, {"someNeighbourStop", 10}, {"thirdNeighbourStop", 2}};
        const auto input = Json::Map{{"name", "stopWithRoadDistances"},
                                     {"latitude", 30.456993},
                                     {"longitude", -30.45632},
                                     {"road_distances", roadDistancesJson}};

        const vector<Distance> expectedParsedRoadDistances = {
            {"anotherNeighbourStop", 57}, {"someNeighbourStop", 10}, {"thirdNeighbourStop", 2}};
        const Stop expected = {.name = "stopWithRoadDistances",
                               .position = {30.456993, -30.45632},
                               .distances = expectedParsedRoadDistances};

        ASSERT_EQUAL(Stop::parseFrom(input), expected);
    }
    {
        const auto input = Json::Map{
            {"name", "stopWithoutRoadDistances"}, {"latitude", 3.294665}, {"longitude", 47.733878}};
        const Stop expected = {
            .name = "stopWithoutRoadDistances", .position = {3.294665, 47.733878}, .distances = {}};
        ASSERT_EQUAL(Stop::parseFrom(input), expected);
    }
}

void run()
{
    TestRunner tr;
    RUN_TEST(tr, testBusParsing);
    RUN_TEST(tr, testStopParsing);
}
} // namespace Tests
} // namespace MakeBaseRequests
