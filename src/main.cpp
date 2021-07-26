#include "baseRequests.h"
#include "json.h"
#include "statRequests.h"
#include "transportCatalog.h"
#include "utils.h"

#include <fstream>

using namespace std;

int main(int argc, const char* argv[])
{
    const auto baseRequestsJsonTree = Json::load(cin);
    const auto& baseRequestsMap = baseRequestsJsonTree.getRoot().asMap();
    const auto baseRequests =
        BaseRequests::parseRequests(baseRequestsMap.at("base_requests").asArray());
    const auto& routingSettings = baseRequestsMap.at("routing_settings").asMap();
    TransportCatalog database(baseRequests, routingSettings);

    const auto statRequestsJsonTree = Json::load(cin);
    const auto& statRequestsMap = statRequestsJsonTree.getRoot().asMap();
    const auto& statRequests = statRequestsMap.at("stat_requests").asArray();
    const auto responses = StatRequests::processAll(database, statRequests);
    Json::printValue(responses, cout);
    cout << endl;

    return 0;
}
