#include "baseRequests.h"
#include "json.h"
#include "statRequests.h"
#include "transportCatalog.h"
#include "utils.h"

#include <fstream>

using namespace std;

namespace
{
constexpr auto WrongParametrsMsg("Usage: transport_catalog [make_base|process_requests]\n");

string readFileData(const string& fileName)
{
    ifstream file(fileName, ios::binary | ios::ate);
    ASSERT_WITH_MESSAGE(file, "can't read open the file " + fileName);
    const ifstream::pos_type endPos = file.tellg();
    file.seekg(0, ios::beg);

    string data(endPos, '\0');
    file.read(&data[0], endPos);
    return data;
}
}

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        cerr << WrongParametrsMsg;
        return 5;
    }

    const auto inputJsonTree = Json::load(cin);
    const auto& inputMap = inputJsonTree.getRoot().asMap();

    const string& serialisationFileName =
        inputMap.at("serialization_settings").asMap().at("file").asString();

    const string_view mode(argv[1]);
    if (mode == "make_base")
    {
        const auto baseRequests =
            BaseRequests::parseRequests(inputMap.at("base_requests").asArray());
        const auto& routingSettings = inputMap.at("routing_settings").asMap();
        TransportCatalog database(baseRequests, routingSettings);
        ofstream(serialisationFileName) << database.serialize();
    }
    else if (mode == "process_requests")
    {
        const auto database =
            TransportCatalog::deserialize(readFileData(serialisationFileName));
        const auto& statRequests = inputMap.at("stat_requests").asArray();
        const auto responses = StatRequests::processAll(database, statRequests);
        Json::printValue(responses, cout);
        cout << endl;
    }
    else
    {
        cerr << WrongParametrsMsg;
        return 5;
    }

    return 0;
}
