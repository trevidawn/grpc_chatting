cmake_minimum_required(VERSION 3.15)
project(chat_client)

set(CMAKE_CXX_STANDARD 14)

# custom variable
set(_binary chat_client)
set(PROTO_NAME chat)

find_package(Threads REQUIRED)

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
if(CMAKE_CROSSCOMPILING)
    find_program(_PROTOBUF_PROTOC protoc)
else()
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()

# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)
if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()

# Proto file
get_filename_component(proto "../protos/${PROTO_NAME}.proto" ABSOLUTE)
get_filename_component(proto_path "${proto}" PATH)


# # Generated sources
set(proto_srcs "${proto_path}/gen/${PROTO_NAME}.pb.cc")
set(proto_hdrs "${proto_path}/gen/${PROTO_NAME}.pb.h")
set(grpc_srcs "${proto_path}/gen/${PROTO_NAME}.grpc.pb.cc")
set(grpc_hdrs "${proto_path}/gen/${PROTO_NAME}.grpc.pb.h")
add_custom_command(
        OUTPUT "${proto_srcs}" "${proto_hdrs}" "${grpc_srcs}" "${grpc_hdrs}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "../protos/gen"
        --cpp_out "../protos/gen"
        -I "${proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${proto}"
        DEPENDS "${proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}/protos/gen")


add_executable(${_binary} main.cpp chat_view.cpp chat_client.cpp ${proto_srcs} ${grpc_srcs})
target_link_libraries(${_binary} ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

