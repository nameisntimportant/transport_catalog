#pragma once

#include "json.h"
#include "transportCatalog.h"

#include <string>
#include <variant>

namespace StatRequests
{
struct Stop
{
    std::string name;

    Json::Map process(const TransportCatalog& database) const;
};

struct Bus
{
    std::string name;

    Json::Map process(const TransportCatalog& database) const;
};

struct Route
{
    std::string from;
    std::string to;

    Json::Map process(const TransportCatalog& database) const;
};

std::variant<Stop, Bus, Route> read(const Json::Map& attrs);

Json::Array processAll(const TransportCatalog& database, const Json::Array& requests);
} // namespace Requests
