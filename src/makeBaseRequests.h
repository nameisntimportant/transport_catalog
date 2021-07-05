#pragma once

#include "json.h"

#include <map>
#include <string>
#include <variant>
#include <vector>

namespace MakeBaseRequests
{
struct Coordinates
{
    double latitude = 0;
    double longitude = 0;
};

struct Distance
{
    std::string destination;
    size_t length = 0;
};

struct Stop
{
    std::string name;
    Coordinates position;
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

using ParsedRequest = std::variant<Stop, Bus>;
using ParsedRequests = std::vector<ParsedRequest>;
ParsedRequests parseRequests(const Json::Array& nodes);
} // namespace MakeBaseRequests
