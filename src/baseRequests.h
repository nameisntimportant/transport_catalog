#pragma once

#include "json.h"
#include "sphere.h"

#include <map>
#include <string>
#include <variant>
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
bool operator==(const Stop& lhs, const Stop& rhs);
std::ostream& operator<<(std::ostream& os, const Stop& stop);

struct Bus
{
    std::string name;
    std::vector<std::string> stops;

    static Bus parseFrom(const Json::Map& attrs);
};
bool operator==(const Bus& lhs, const Bus& rhs);
std::ostream& operator<<(std::ostream& os, const Bus& bus);

using ParsedStops = std::vector<Stop>;
using ParsedBuses = std::vector<Bus>;
struct ParsedRequests
{
    ParsedStops stops;
    ParsedBuses buses;
};
ParsedRequests parseRequests(const Json::Array& nodes);
} // namespace BaseRequests
