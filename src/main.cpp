#include "json.h"
#include "tests/jsonTestSuite.h"
#include "tests/makeBaseRequestsTestSuit.h"

using namespace std;

void runTests()
{
    Json::Tests::run();
    MakeBaseRequests::Tests::run();
}

int main(int argc, const char* argv[])
{
    runTests();
    return 0;
}
