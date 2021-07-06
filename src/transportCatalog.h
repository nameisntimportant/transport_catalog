#pragma once

#include "makeBaseRequests.h"
#include "routeDistancesDict.h"
#include "sphere.h"

#include <set>
#include <string>
#include <unordered_map>

namespace Responces
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
} // namespace Responces

class TransportCatalog
{
private:
    using Bus = MakeBaseRequests::Bus;
    using Stop = MakeBaseRequests::Stop;
    using PointsMap = std::unordered_map<std::string, Sphere::Point>;

public:
    TransportCatalog(const MakeBaseRequests::ParsedRequests& data);

    const Responces::Stop* getStop(const std::string& name) const;
    const Responces::Bus* getBus(const std::string& name) const;

private:
    static PointsMap getStopCoordinates(const MakeBaseRequests::ParsedStops& stops);
    static RouteDistancesMap getRouteDistances(const MakeBaseRequests::ParsedStops& stops);

    static size_t calculateRoadRouteLength(const std::vector<std::string>& stops,
                                           const RouteDistancesMap& distancesDict);
    static double calculateOrthodromicRouteLength(const std::vector<std::string>& stops,
                                                  const PointsMap& stopsCoordinates);

    std::unordered_map<std::string, Responces::Stop> stops_;
    std::unordered_map<std::string, Responces::Bus> buses_;
};
