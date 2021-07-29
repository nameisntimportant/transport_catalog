set(UNIT_TESTS_PROJECT_SRCS
    main.cpp
    jsonTestSuite.cpp
    baseRequestsTestSuite.cpp
    sphereTestSuite.cpp
    graphTestSuite.cpp
    routerTestSuite.cpp
    transportRouterTestSuite.cpp)

set(UNIT_TESTS_PROJECT_HDRS
    jsonTestSuite.h
    baseRequestsTestSuite.h
    sphereTestSuite.h
    graphTestSuite.h
    routerTestSuite.h
    transportRouterTestSuite.h
    testRunner.h)

set(UNDER_TEST_SRCS
    ${SRC_DIRECTORY}/json.cpp
    ${SRC_DIRECTORY}/baseRequests.cpp
    ${SRC_DIRECTORY}/sphere.cpp
    ${SRC_DIRECTORY}/transportRouter.cpp
    ${UTILS_DIRECTORY}/utils.cpp)

set(UNDER_TEST_HDRS
    ${SRC_DIRECTORY}/json.h
    ${SRC_DIRECTORY}/baseRequests.h
    ${SRC_DIRECTORY}/sphere.h
    ${SRC_DIRECTORY}/graph.h
    ${SRC_DIRECTORY}/router.h
    ${SRC_DIRECTORY}/routeDistancesDict.h
    ${SRC_DIRECTORY}/transportRouter.h)

add_executable(${UNIT_TESTS_PROJECT} ${UNIT_TESTS_PROJECT_SRCS} ${UNIT_TESTS_PROJECT_HDRS}
               ${UNDER_TEST_SRCS} ${UNDER_TEST_HDRS})

target_include_directories(${UNIT_TESTS_PROJECT} PRIVATE ${SRC_DIRECTORY} ${UTILS_DIRECTORY})
