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
    ${TESTS_DIRECTORY}/jsonTestSuite.cpp
    ${UTILS_DIRECTORY}/utils.cpp)

set (PROJECT_HDRS
    ${SRC_DIRECTORY}/json.h
    ${TESTS_DIRECTORY}/jsonTestSuite.h
    ${UTILS_DIRECTORY}/utils.h
    ${UTILS_DIRECTORY}/log.h
    ${UTILS_DIRECTORY}/testRunner.h)

add_executable(${PROJECT_NAME} ${PROJECT_SRCS} ${PROJECT_HDRS})
