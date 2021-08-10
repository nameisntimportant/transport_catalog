#include "baseRequests.h"
#include "utils.h"

using namespace std;

namespace BaseRequests
{
Stop Stop::parseFrom(const Json::Map& attrs)
{
    Sphere::Point position = {.latitude = attrs.at("latitude").asDouble(),
                              .longitude = attrs.at("longitude").asDouble()};
    Stop stop = {.name = attrs.at("name").asString(), .position = position, .distances = {}};

    if (attrs.count("road_distances") > 0)
    {
        for (const auto& [neighbourStop, distanceNode] : attrs.at("road_distances").asMap())
        {
            const auto distance = distanceNode.asInt();
            ASSERT_WITH_MESSAGE(distance >= 0, "distance can't be negative: " + to_string(distance));
            stop.distances.push_back({neighbourStop, static_cast<unsigned int>(distance)});
        }
    }
    return stop;
}

vector<string> parseStops(const Json::Array& stopNodes, bool isRoundtrip)
{
    vector<string> stops;
    stops.reserve(stopNodes.size());

    for (const Json::Node& stopNode : stopNodes)
    {
        stops.push_back(stopNode.asString());
    }
    if (isRoundtrip || stops.size() <= 1)
    {
        return stops;
    }

    stops.reserve(stops.size() * 2 - 1); // end stop is not repeated
    for (size_t stopIndex = stops.size() - 1; stopIndex > 0; --stopIndex)
    {
        stops.push_back(stops[stopIndex - 1]);
    }

    return stops;
}

Bus Bus::parseFrom(const Json::Map& attrs)
{
    Bus result;
    result.name = attrs.at("name").asString();

    if (attrs.count("stops") > 0)
    {
        const auto& stops = attrs.at("stops").asArray();
        if (!stops.empty())
        {
            result.stops = parseStops(stops, attrs.at("is_roundtrip").asBool());
        }
    }
    return result;
}

ParsedRequests parseRequests(const Json::Array& nodes)
{
    ParsedRequests result;

    for (const Json::Node& node : nodes)
    {
        const auto& nodeDict = node.asMap();
        if (nodeDict.at("type").asString() == "Stop")
        {
            result.stops.push_back(Stop::parseFrom(nodeDict));
        }
        else
        {
            result.buses.push_back(Bus::parseFrom(nodeDict));
        }
    }

    return result;
}
} // namespace BaseRequests
