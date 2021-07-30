set(CMAKE_MIN_VERSION 3.5)

set(TARGET proto_lib)

set(PROJECT_PROTOS
    transport_catalog.proto
    transport_router.proto
    graph.proto)

find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROJECT_PROTOS})
add_library(${TARGET} ${PROTO_SRCS} ${PROTO_HDRS})

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${TARGET} PRIVATE protobuf::libprotobuf)

