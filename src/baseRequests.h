#pragma once

#include "json.h"
#include "sphere.h"

#include <string>
#include <vector>

namespace BaseRequests
{
struct Distance
{
    std::string destination;
    size_t length = 0;
};

struct Stop
{
    std::string name;
    Sphere::Point position;
    std::vector<Distance> distances;

    static Stop parseFrom(const Json::Map& attrs);
};

struct Bus
{
    std::string name;
    std::vector<std::string> stops;

    static Bus parseFrom(const Json::Map& attrs);
};

using ParsedStops = std::vector<Stop>;
using ParsedBuses = std::vector<Bus>;
struct ParsedRequests
{
    ParsedStops stops;
    ParsedBuses buses;
};
ParsedRequests parseRequests(const Json::Array& nodes);
} // namespace BaseRequests
