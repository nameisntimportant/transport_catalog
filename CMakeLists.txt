set(CMAKE_MIN_VERSION 3.5)
cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION})

project(transport_guide LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIRECTORY ${CMAKE_SOURCE_DIR}/src)
set(TESTS_DIRECTORY ${SRC_DIRECTORY}/tests)
set(UTILS_DIRECTORY ${SRC_DIRECTORY}/utils)

include_directories(${UTILS_DIRECTORY})

set (PROJECT_SRCS
    ${SRC_DIRECTORY}/main.cpp
    ${SRC_DIRECTORY}/json.cpp
    ${SRC_DIRECTORY}/baseRequests.cpp
    ${SRC_DIRECTORY}/transportCatalog.cpp
    ${SRC_DIRECTORY}/sphere.cpp
    ${SRC_DIRECTORY}/statRequests.cpp
    ${TESTS_DIRECTORY}/jsonTestSuite.cpp
    ${TESTS_DIRECTORY}/baseRequestsTestSuite.cpp
    ${TESTS_DIRECTORY}/sphereTestSuite.cpp
    ${TESTS_DIRECTORY}/graphTestSuite.cpp
    ${TESTS_DIRECTORY}/routerTestSuite.cpp
    ${UTILS_DIRECTORY}/utils.cpp)

set (PROJECT_HDRS
    ${SRC_DIRECTORY}/json.h
    ${SRC_DIRECTORY}/baseRequests.h
    ${SRC_DIRECTORY}/transportCatalog.h
    ${SRC_DIRECTORY}/sphere.h
    ${SRC_DIRECTORY}/statRequests.h
    ${SRC_DIRECTORY}/graph.h
    ${SRC_DIRECTORY}/router.h
    ${TESTS_DIRECTORY}/jsonTestSuite.h
    ${TESTS_DIRECTORY}/baseRequestsTestSuite.h
    ${TESTS_DIRECTORY}/sphereTestSuite.h
    ${TESTS_DIRECTORY}/graphTestSuite.h
    ${TESTS_DIRECTORY}/routerTestSuite.h
    ${UTILS_DIRECTORY}/utils.h
    ${UTILS_DIRECTORY}/log.h
    ${UTILS_DIRECTORY}/testRunner.h)

add_executable(${PROJECT_NAME} ${PROJECT_SRCS} ${PROJECT_HDRS})
