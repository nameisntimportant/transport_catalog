#include "json.h"
#include "log.h"
#include "baseRequests.h"
#include "statRequests.h"
#include "transportCatalog.h"
#include "utils.h"

#include "testRunner.h"
#include "tests/jsonTestSuite.h"
#include "tests/baseRequestsTestSuite.h"
#include "tests/sphereTestSuite.h"
#include "tests/graphTestSuite.h"
#include "tests/routerTestSuite.h"

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

// TODO: move it to a python script
void generalTest()
{
    for (size_t i = 1; i < 2; i++)
    {
        auto baseRequestsInput = openFileAsInputStream("../transport_catalog/testData/make_base_" +
                                              to_string(i) + ".json");
        const auto baseRequestsJsonTree = Json::load(baseRequestsInput);
        const auto& baseRequestsMap = baseRequestsJsonTree.getRoot().asMap();
        const auto baseRequests =
            BaseRequests::parseRequests(baseRequestsMap.at("base_requests").asArray());
        const TransportCatalog database(baseRequests);

        auto statRequestsInput = openFileAsInputStream(
            "../transport_catalog/testData/stat_requests_" + to_string(i) + ".json");
        const auto statRequestsJsonTree = Json::load(statRequestsInput);
        const auto& statRequestsMap = statRequestsJsonTree.getRoot().asMap();
        const auto& statRequests = statRequestsMap.at("stat_requests").asArray();
        const auto responses = StatRequests::processAll(database, statRequests);

        ostringstream output;
        Json::printValue(responses, output);
        output << std::endl;

        string expectedOutput =
            readWholeFile("../transport_catalog/testData/expected_" + to_string(i) + ".json");
        ASSERT_EQUAL(expectedOutput, output.str());
    }
}

void runTests()
{
    Json::Tests::run();
    BaseRequests::Tests::run();
    Sphere::Tests::run();
    Graph::Tests::runGraphTests();
    Graph::Tests::runRouterTests();

    TestRunner tr;
    RUN_TEST(tr, generalTest);
}

int main(int argc, const char* argv[])
{
    runTests();

    return 0;
}
