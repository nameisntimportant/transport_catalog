#include "json.h"
#include "tests/jsonTestSuite.h"

using namespace std;

void runTests()
{
    Json::Tests::run();
}

int main(int argc, const char* argv[])
{
    runTests();
    return 0;
}
