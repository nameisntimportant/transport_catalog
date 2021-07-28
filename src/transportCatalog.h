#pragma once

#include "baseRequests.h"
#include "routeDistancesDict.h"
#include "sphere.h"
#include "transportRouter.h"

#include <set>
#include <string>
#include <unordered_map>

namespace Responses
{
struct Stop
{
    std::set<std::string> busNames;
};

struct Bus
{
    size_t stopCount = 0;
    size_t uniqueStopCount = 0;
    size_t roadRouteLength = 0;
    double orthodromicRouteLength = 0.0;
};

using Route = std::optional<TransportRouter::RouteStats>;
} // namespace Responses

class TransportCatalog
{
private:
    using Bus = Responses::Bus;
    using Stop = Responses::Stop;
    using Route = Responses::Route;
    using PointsMap = std::unordered_map<std::string, Sphere::Point>;

public:
    TransportCatalog(const BaseRequests::ParsedRequests& data, const Json::Map& routingSettings);

    const Stop* getStop(const std::string& name) const;
    const Bus* getBus(const std::string& name) const;
    Route findRoute(const std::string& from, const std::string& to) const;

    std::string serialize() const;
    static TransportCatalog deserialize(const std::string& data);

private:
    TransportCatalog() = default;

    static PointsMap getStopCoordinates(const BaseRequests::ParsedStops& stops);
    static RouteDistancesMap getRouteDistances(const BaseRequests::ParsedStops& stops);

    static size_t calculateRoadRouteLength(const std::vector<std::string>& stops,
                                           const RouteDistancesMap& distancesDict);
    static double calculateOrthodromicRouteLength(const std::vector<std::string>& stops,
                                                  const PointsMap& stopsCoordinates);

    std::unique_ptr<TransportRouter> router_;
    std::unordered_map<std::string, Stop> stops_;
    std::unordered_map<std::string, Bus> buses_;
};
