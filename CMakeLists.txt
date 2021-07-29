set(CMAKE_MIN_VERSION 3.5)
cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION})

project(transport_catalog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(RUN_UNIT_TESTS_AUTOMATICALLY "Automatically run unit tests after building transport catalog" OFF)
option(RUN_INTEGRATION_TESTS_AUTOMATICALLY "Automatically run integration tests after building transport catalog" OFF)

set(SRC_DIRECTORY ${CMAKE_SOURCE_DIR}/src)
set(UTILS_DIRECTORY ${SRC_DIRECTORY}/utils)

set(UNIT_TESTS_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)
set(UNIT_TESTS_PROJECT ${PROJECT_NAME}_tests)

set(INTEGRATION_TEST_DIRECTORY ${CMAKE_SOURCE_DIR}/integrationTest)
set(INTEGRATION_TEST_SCRIPT ${INTEGRATION_TEST_DIRECTORY}/integrationTest.py)

add_subdirectory(${SRC_DIRECTORY})
add_subdirectory(${UNIT_TESTS_DIRECTORY})
