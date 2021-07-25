#include "jsonTestSuite.h"
#include "baseRequestsTestSuite.h"
#include "graphTestSuite.h"
#include "routerTestSuite.h"
#include "sphereTestSuite.h"
#include "transportRouterTestSuite.h"

int main(int argc, const char* argv[])
{
    Json::Tests::run();
    BaseRequests::Tests::run();
    Sphere::Tests::run();
    Graph::Tests::runGraphTests();
    Graph::Tests::runRouterTests();
    Tests::runTransportRouterTests();

    return 0;
}
