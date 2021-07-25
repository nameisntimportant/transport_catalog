#include "baseRequests.h"
#include "json.h"
#include "statRequests.h"
#include "transportCatalog.h"
#include "utils.h"

#include <fstream>

using namespace std;

ifstream openFileAsInputStream(const string& fileName, ios_base::openmode mode = ios::in)
{
    ifstream fileStream(fileName, mode);
    ASSERT_WITH_MESSAGE(fileStream, ("can't open the file - it doesn't exist:" + fileName));
    return fileStream;
}

string readWholeFile(const string& fileName)
{
    ifstream fileStream = openFileAsInputStream(fileName, ios::ate);
    const ifstream::pos_type endPos = fileStream.tellg();
    fileStream.seekg(0, ios::beg);

    string data(endPos, '\0');
    fileStream.read(&data[0], endPos);
    return data;
}

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
