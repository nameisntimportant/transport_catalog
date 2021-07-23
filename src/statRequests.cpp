#include "statRequests.h"
#include "json.h"
#include "makeVisitor.h"
#include "transportCatalog.h"
#include "utils.h"

#include <vector>

using namespace std;

namespace
{
const Json::Map NotFoundErrorResponse = {{"error_message"s, Json::Node("not found"s)}};
} // namespace

namespace StatRequests
{
variant<Stop, Bus, Route> read(const Json::Map& attrs)
{
    const string& type = attrs.at("type").asString();
    if (type == "Bus")
    {
        return Bus{attrs.at("name").asString()};
    }
    else if (type == "Stop")
    {
        return Stop{attrs.at("name").asString()};
    }
    else if (type == "Route")
    {
        return Route{attrs.at("from").asString(), attrs.at("to").asString()};
    }
    UNREACHABLE("unknown type of request: "s + type);
}

Json::Array processAll(const TransportCatalog& database, const Json::Array& requestNodes)
{
    Json::Array responses;
    responses.reserve(requestNodes.size());

    // TODO: split the logic of processing different requests
    for (const Json::Node& rn : requestNodes)
    {
        const auto request = read(rn.asMap());

        Json::Map responseDict = visit(
            make_visitor(
                [&database](const Stop& stopRequest) {
                    const auto* stop = database.getStop(stopRequest.name);
                    if (!stop)
                    {
                        return NotFoundErrorResponse;
                    }

                    Json::Array busNodes;
                    busNodes.reserve(stop->busNames.size());
                    for (const auto& busName : stop->busNames)
                    {
                        busNodes.emplace_back(busName);
                    }
                    return Json::Map{{"buses", Json::Node(move(busNodes))}};
                },
                [&database](const Bus& busRequest) {
                    const auto* bus = database.getBus(busRequest.name);
                    if (!bus)
                    {
                        return NotFoundErrorResponse;
                    }

                    return Json::Map{
                        {"stop_count", Json::Node(static_cast<int>(bus->stopCount))},
                        {"unique_stop_count", Json::Node(static_cast<int>(bus->uniqueStopCount))},
                        {"route_length", Json::Node(static_cast<int>(bus->roadRouteLength))},
                        {"curvature",
                         Json::Node(bus->roadRouteLength / bus->orthodromicRouteLength)},
                    };
                },
                [&database](const Route& routeRequest) {
                    const auto route = database.findRoute(routeRequest.from, routeRequest.to);
                    if (!route)
                    {
                        return NotFoundErrorResponse;
                    }

                    Json::Map dict;
                    dict["total_time"] = Json::Node(route->totalTime);
                    Json::Array items;
                    items.reserve(route->routeElements.size());
                    for (const auto& element : route->routeElements)
                    {
                        auto waitElement = Json::Map{{"type", Json::Node("Wait"s)},
                                                     {"stop_name", Json::Node(element.from)},
                                                     {"time", Json::Node(element.waitTime)}};
                        items.push_back(std::move(waitElement));

                        auto busElement = Json::Map{
                            {"type", Json::Node("Bus"s)},
                            {"bus", Json::Node(element.bus)},
                            {"time", Json::Node(element.transitTime)},
                            {"span_count", Json::Node(static_cast<int>(element.spanCount))}};

                        items.push_back(std::move(busElement));
                    }

                    dict["items"] = move(items);

                    return dict;
                }),
            request);

        responseDict["request_id"] = Json::Node(rn.asMap().at("id").asInt());
        responses.push_back(Json::Node(responseDict));
    }
    return responses;
}
} // namespace StatRequests
