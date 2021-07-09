#pragma once

#include "baseRequests.h"
#include "routeDistancesDict.h"
#include "sphere.h"

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
} // namespace Responses

class TransportCatalog
{
private:
    using Bus = Responses::Bus;
    using Stop = Responses::Stop;
    using PointsMap = std::unordered_map<std::string, Sphere::Point>;

public:
    TransportCatalog(const BaseRequests::ParsedRequests& data);

    const Stop* getStop(const std::string& name) const;
    const Bus* getBus(const std::string& name) const;

private:
    static PointsMap getStopCoordinates(const BaseRequests::ParsedStops& stops);
    static RouteDistancesMap getRouteDistances(const BaseRequests::ParsedStops& stops);

    static size_t calculateRoadRouteLength(const std::vector<std::string>& stops,
                                           const RouteDistancesMap& distancesDict);
    static double calculateOrthodromicRouteLength(const std::vector<std::string>& stops,
                                                  const PointsMap& stopsCoordinates);

    std::unordered_map<std::string, Stop> stops_;
    std::unordered_map<std::string, Bus> buses_;
};
