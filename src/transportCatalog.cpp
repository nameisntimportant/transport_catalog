#include "transportCatalog.h"
#include "utils.h"

#include "transport_catalog.pb.h"

using namespace std;

TransportCatalog::TransportCatalog(const BaseRequests::ParsedRequests& data,
                                   const Json::Map& routingSettings)
{
    for (const auto& stop : data.stops)
    {
        stops_.insert({stop.name, {}});
    }

    const auto stopsCoordinates = getStopCoordinates(data.stops);
    const auto routeDistances = getRouteDistances(data.stops);
    for (const auto& bus : data.buses)
    {
        buses_[bus.name] = Bus{
            .stopCount = bus.stops.size(),
            .uniqueStopCount = calculateUniqueItemsCount(asRange(bus.stops)),
            .roadRouteLength = calculateRoadRouteLength(bus.stops, routeDistances),
            .orthodromicRouteLength = calculateOrthodromicRouteLength(bus.stops, stopsCoordinates)};

        for (const string& stopName : bus.stops)
        {
            stops_.at(stopName).busNames.insert(bus.name);
        }
    }

    router_ = make_unique<TransportRouter>(data.buses, routeDistances, routingSettings);
}

const Responses::Stop* TransportCatalog::getStop(const string& name) const
{
    return getValuePointer(stops_, name);
}

const Responses::Bus* TransportCatalog::getBus(const string& name) const
{
    return getValuePointer(buses_, name);
}

Responses::Route TransportCatalog::findRoute(const std::string& from, const std::string& to) const
{
    return router_->findRoute(from, to);
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

string TransportCatalog::serialize() const
{
    TCProto::TransportCatalog proto;

    for (const auto& [name, stop] : stops_)
    {
        TCProto::Stop& stopProto = *proto.add_stops();
        stopProto.set_name(name);
        for (const string& busName : stop.busNames)
        {
            stopProto.add_bus_names(busName);
        }
    }

    for (const auto& [name, bus] : buses_)
    {
        TCProto::Bus& busProto = *proto.add_buses();
        busProto.set_name(name);
        busProto.set_stop_count(bus.stopCount);
        busProto.set_unique_stop_count(bus.uniqueStopCount);
        busProto.set_road_route_length(bus.roadRouteLength);
        busProto.set_orthodromic_route_length(bus.orthodromicRouteLength);
    }

    router_->serialize(*proto.mutable_router());

    return proto.SerializeAsString();
}

TransportCatalog TransportCatalog::deserialize(const string& data)
{
    TCProto::TransportCatalog proto;
    assert(proto.ParseFromString(data));

    TransportCatalog catalog;

    for (const TCProto::Stop& stopProto : proto.stops())
    {
        Stop& stop = catalog.stops_[stopProto.name()];
        for (const string& busName : stopProto.bus_names())
        {
            stop.busNames.insert(busName);
        }
    }

    for (const TCProto::Bus& busProto : proto.buses())
    {
        Bus& bus = catalog.buses_[busProto.name()];
        bus.stopCount = busProto.stop_count();
        bus.uniqueStopCount = busProto.unique_stop_count();
        bus.roadRouteLength = busProto.road_route_length();
        bus.orthodromicRouteLength = busProto.orthodromic_route_length();
    }

    catalog.router_ = TransportRouter::deserialize(proto.router());

    return catalog;
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
