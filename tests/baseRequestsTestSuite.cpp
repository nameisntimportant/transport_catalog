#include "baseRequestsTestSuite.h"

#include "baseRequests.h"
#include "json.h"

#include "testRunner.h"

using namespace std;

namespace
{
bool operator==(const BaseRequests::Bus& lhs, const BaseRequests::Bus& rhs)
{
    if (lhs.stops.size() != rhs.stops.size())
    {
        return false;
    }
    for (size_t i = 0; i < lhs.stops.size(); i++)
    {
        if (lhs.stops[i] != rhs.stops[i])
        {
            return false;
        }
    }
    return lhs.name == rhs.name;
}

ostream& operator<<(ostream& os, const BaseRequests::Bus& bus)
{
    os << "busName: " << bus.name << " stops: {";
    for (const auto& stopName : bus.stops)
    {
        os << stopName << ' ';
    }
    return os << " }";
}

bool operator==(const BaseRequests::Stop& lhs, const BaseRequests::Stop& rhs)
{
    if (lhs.distances.size() != rhs.distances.size())
    {
        return false;
    }

    for (size_t i = 0; i < lhs.distances.size(); i++)
    {
        if (lhs.distances[i].destination != rhs.distances[i].destination ||
            lhs.distances[i].length != rhs.distances[i].length)
        {
            return false;
        }
    }

    return lhs.name == rhs.name && lhs.position.latitude == rhs.position.latitude &&
           lhs.position.longitude == rhs.position.longitude;
}

ostream& operator<<(ostream& os, const BaseRequests::Stop& stop)
{
    os << "stopName: " << stop.name << " distances: { ";
    for (const auto& distance : stop.distances)
    {
        os << distance.destination << ": " << distance.length << ' ';
    }
    os << "} latitude: " << stop.position.latitude << " longitude: " << stop.position.longitude;
    return os;
}
} // namespace

namespace BaseRequests
{
namespace Tests
{
void testBusParsing()
{
    {
        const auto input = Json::Map{{"name", "NonRoundtripBusABC"},
                                     {"stops", Json::Array{"stopA", "stopB", "stopC"}},
                                     {"is_roundtrip", false}};

        const Bus expected = {.name = "NonRoundtripBusABC",
                              .stops = {"stopA", "stopB", "stopC", "stopB", "stopA"}};

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
        ASSERT_EQUAL(Bus::parseFrom(Json::Map{{"name", "NoStopsBus"}}),
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
} // namespace BaseRequests
