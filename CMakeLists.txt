set(CMAKE_MIN_VERSION 3.5)
cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION})

project(transport_catalog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message ("building was tested with GCC only")
endif()

option(RUN_UNIT_TESTS_AUTOMATICALLY "Automatically run unit tests after building transport catalog" OFF)
option(RUN_INTEGRATION_TESTS_AUTOMATICALLY "Automatically run integration tests after building transport catalog" OFF)
option(ENABLE_RUNTIME_CHECKS "Compile with sanitizers and debugging flags" OFF)

add_compile_options(
    -Wall -Wextra -O2

    -pedantic -std=c++17

    -Wsign-conversion -Wsign-promo -Wfloat-equal -Wenum-compare
    -Wshadow -Wformat=2 -Wconversion -Wshift-overflow=2 -Wduplicated-cond -Wlogical-op
    -Wcast-qual -Wcast-align -Wold-style-cast -Wredundant-decls
    -Woverloaded-virtual -Wctor-dtor-privacy -Wnon-virtual-dtor
)

if(ENABLE_RUNTIME_CHECKS)
    add_compile_options(
        -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2
        -fsanitize=address -fsanitize=undefined -fno-sanitize-recover
        -fstack-protector
    )
    add_link_options(-fsanitize=address -fsanitize=undefined)
endif()

set(SRC_DIRECTORY ${CMAKE_SOURCE_DIR}/src)
set(UTILS_DIRECTORY ${SRC_DIRECTORY}/utils)                             # got to set it in top-level file since we include it in ${UNIT_TESTS_PROJECT}

set(PROTO_DIRECTORY ${SRC_DIRECTORY}/proto)

set(UNIT_TESTS_DIRECTORY ${CMAKE_SOURCE_DIR}/unit_tests_src)
set(UNIT_TESTS_PROJECT ${PROJECT_NAME}_unit_tests)                      # got to set it in top-level file since we run it from ${SRC_DIRECTORY}

set(INTEGRATION_TEST_DIRECTORY ${CMAKE_SOURCE_DIR}/integration_test)
set(INTEGRATION_TEST_SCRIPT ${INTEGRATION_TEST_DIRECTORY}/integration_test.py)

add_subdirectory(${PROTO_DIRECTORY})
add_subdirectory(${UNIT_TESTS_DIRECTORY})                               # compile it first, since we run it from ${SRC_DIRECTORY}
add_subdirectory(${SRC_DIRECTORY})
