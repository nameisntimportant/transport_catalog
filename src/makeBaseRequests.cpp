#include "makeBaseRequests.h"

using namespace std;

namespace MakeBaseRequests
{
Stop Stop::parseFrom(const Json::Map& attrs)
{
    Sphere::Point position = {.latitude = attrs.at("latitude").asDouble(),
                            .longitude = attrs.at("longitude").asDouble()};
    Stop stop = {.name = attrs.at("name").asString(), .position = position};

    if (attrs.count("road_distances") > 0)
    {
        for (const auto& [neighbourStop, distanceNode] : attrs.at("road_distances").asMap())
        {
            stop.distances.push_back(
                {neighbourStop, static_cast<unsigned int>(distanceNode.asInt())});
        }
    }
    return stop;
}

bool operator==(const Stop& lhs, const Stop& rhs)
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

std::ostream& operator<<(std::ostream& os, const Stop& stop)
{
    os << "stopName: " << stop.name << " distances: { ";
    for (const auto& distance : stop.distances)
    {
        os << distance.destination << ": " << distance.length << ' ';
    }
    os << "} latitude: " << stop.position.latitude << " longitude: " << stop.position.longitude;
    return os;
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

bool operator==(const Bus& lhs, const Bus& rhs)
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

std::ostream& operator<<(std::ostream& os, const Bus& bus)
{
    os << "busName: " << bus.name << " stops: {";
    for (const auto& stopName : bus.stops)
    {
        os << stopName << ' ';
    }
    return os << " }";
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
} // namespace MakeBaseRequests
