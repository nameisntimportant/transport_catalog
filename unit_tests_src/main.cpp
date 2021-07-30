#include "jsonTestSuite.h"
#include "baseRequestsTestSuite.h"
#include "graphTestSuite.h"
#include "routerTestSuite.h"
#include "sphereTestSuite.h"
#include "transportRouterTestSuite.h"
#include <iostream>

int main()
{
    std::cout << "starting unit tests\n";

    Json::Tests::run();
    BaseRequests::Tests::run();
    Sphere::Tests::run();
    Graph::Tests::runGraphTests();
    Graph::Tests::runRouterTests();
    Tests::runTransportRouterTests();

    std::cout << "all tests OK\n";

    return 0;
}
