set(TARGET ${PROJECT_NAME})

set(PROJECT_SRCS
    main.cpp
    json.cpp
    baseRequests.cpp
    transportCatalog.cpp
    sphere.cpp
    statRequests.cpp
    transportRouter.cpp
    ${UTILS_DIRECTORY}/utils.cpp)

set(PROJECT_HDRS
    json.h
    baseRequests.h
    transportCatalog.h
    sphere.h
    statRequests.h
    graph.h
    router.h
    routeDistancesDict.h
    transportRouter.h
    ${UTILS_DIRECTORY}/utils.h
    ${UTILS_DIRECTORY}/log.h
    ${UTILS_DIRECTORY}/profiler.h)

add_executable(${TARGET} ${PROJECT_SRCS} ${PROJECT_HDRS} ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(${TARGET} PRIVATE ${UTILS_DIRECTORY})
target_link_libraries(${TARGET} PRIVATE proto_lib)

if(RUN_UNIT_TESTS_AUTOMATICALLY)
    add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${UNIT_TESTS_PROJECT})
endif()

set(PATH_TO_BINARY_UNDER_INTEGRATION_TEST ${CMAKE_CURRENT_BINARY_DIR}/${TARGET})
if(RUN_INTEGRATION_TESTS_AUTOMATICALLY)
    add_custom_command(TARGET ${TARGET} POST_BUILD
                       COMMAND ${INTEGRATION_TEST_SCRIPT} ${PATH_TO_BINARY_UNDER_INTEGRATION_TEST}
                       WORKING_DIRECTORY ${INTEGRATION_TEST_DIRECTORY})
endif ()
