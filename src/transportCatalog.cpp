#include "transportCatalog.h"
#include "utils.h"

using namespace std;

TransportCatalog::TransportCatalog(const BaseRequests::ParsedRequests& data)
{
    for (const auto& stop : data.stops)
    {
        stops_.insert({stop.name, {}});
    }

    const auto stopsCoordinates = getStopCoordinates(data.stops);
    const auto routeDistances = getRouteDistances(data.stops);
    for (const auto& bus : data.buses)
    {
        buses_[bus.name] =
            Responses::Bus{bus.stops.size(),
                           calculateUniqueItemsCount(asRange(bus.stops)),
                           calculateRoadRouteLength(bus.stops, routeDistances),
                           calculateOrthodromicRouteLength(bus.stops, stopsCoordinates)};

        for (const string& stopName : bus.stops)
        {
            stops_.at(stopName).busNames.insert(bus.name);
        }
    }
}

const Responses::Stop* TransportCatalog::getStop(const string& name) const
{
    return getValuePointer(stops_, name);
}

const Responses::Bus* TransportCatalog::getBus(const string& name) const
{
    return getValuePointer(buses_, name);
}

TransportCatalog::PointsMap TransportCatalog::getStopCoordinates(
    const BaseRequests::ParsedStops& stops)
{
    PointsMap result;
    for (const auto& stopInput : stops)
    {
        result.emplace(stopInput.name,
                       Sphere::Point{stopInput.position.latitude, stopInput.position.longitude});
    }
    return result;
}

RouteDistancesMap TransportCatalog::getRouteDistances(const BaseRequests::ParsedStops& stops)
{
    RouteDistancesMap result;
    for (const auto& stop : stops)
    {
        for (const auto& distance : stop.distances)
        {
            result[{stop.name, distance.destination}] = distance.length;

            FromTo wayBack = {distance.destination, stop.name};
            if (result.count(wayBack) == 0)
            {
                result[wayBack] = distance.length;
            }
        }
    }
    return result;
}

size_t TransportCatalog::calculateRoadRouteLength(const vector<string>& stops,
                                                  const RouteDistancesMap& distancesDict)
{
    size_t result = 0;
    for (size_t i = 1; i < stops.size(); ++i)
    {
        result += distancesDict.at({stops[i - 1], stops[i]});
    }
    return result;
}

double TransportCatalog::calculateOrthodromicRouteLength(const vector<string>& stops,
                                                         const PointsMap& stopsCoordinates)
{
    double result = 0;
    for (size_t i = 1; i < stops.size(); ++i)
    {
        result +=
            Sphere::distance(stopsCoordinates.at(stops[i - 1]), stopsCoordinates.at(stops[i]));
    }
    return result;
}
