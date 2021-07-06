#include "statRequests.h"
#include "json.h"
#include "makeVisitor.h"
#include "transportCatalog.h"
#include "utils.h"

#include <vector>

using namespace std;

namespace
{
const Json::Map NotFoundErrorResponce = {{"error_message"s, Json::Node("not found"s)}};
} // namespace

namespace StatRequests
{
variant<Stop, Bus> read(const Json::Map& attrs)
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
    UNREACHABLE("unknown type of request: "s + type);
}

Json::Array processAll(const TransportCatalog& database, const Json::Array& requestNodes)
{
    Json::Array responses;
    responses.reserve(requestNodes.size());

    for (const Json::Node& rn : requestNodes)
    {
        const auto request = read(rn.asMap());

        Json::Map responseDict = visit(
            make_visitor(
                [&database](const Stop& stopRequest) {
                    const auto* stop = database.getStop(stopRequest.name);
                    if (!stop)
                    {
                        return NotFoundErrorResponce;
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
                        return NotFoundErrorResponce;
                    }

                    return Json::Map{
                        {"stop_count", Json::Node(static_cast<int>(bus->stopCount))},
                        {"unique_stop_count", Json::Node(static_cast<int>(bus->uniqueStopCount))},
                        {"route_length", Json::Node(static_cast<int>(bus->roadRouteLength))},
                        {"curvature",
                         Json::Node(bus->roadRouteLength / bus->orthodromicRouteLength)},
                    };
                }),
            request);

        responseDict["request_id"] = Json::Node(rn.asMap().at("id").asInt());
        responses.push_back(Json::Node(responseDict));
    }

    return responses;
}

} // namespace StatRequests
