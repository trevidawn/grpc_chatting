cmake_minimum_required(VERSION 3.15)
project(chat_server)

set(CMAKE_CXX_STANDARD 14)

#set(PROTO_NAME chat)

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
get_filename_component(chat_proto "../protos/chat.proto" ABSOLUTE)
get_filename_component(chat_proto_path "${chat_proto}" PATH)

get_filename_component(dbaccessor_proto "../protos/dbaccessor.proto" ABSOLUTE)
get_filename_component(dbaccessor_proto_path "${dbaccessor_proto}" PATH)


# # Generated sources
set(chat_proto_srcs "${chat_proto_path}/gen/cpp/chat.pb.cc")
set(chat_proto_hdrs "${chat_proto_path}/gen/cpp/chat.pb.h")
set(chat_grpc_srcs "${chat_proto_path}/gen/cpp/chat.grpc.pb.cc")
set(chat_grpc_hdrs "${chat_proto_path}/gen/cpp/chat.grpc.pb.h")

set(dbaccessor_proto_srcs "${dbaccessor_proto_path}/gen/cpp/dbaccessor.pb.cc")
set(dbaccessor_proto_hdrs "${dbaccessor_proto_path}/gen/cpp/dbaccessor.pb.h")
set(dbaccessor_grpc_srcs "${dbaccessor_proto_path}/gen/cpp/dbaccessor.grpc.pb.cc")
set(dbaccessor_grpc_hdrs "${dbaccessor_proto_path}/gen/cpp/dbaccessor.grpc.pb.h")

add_custom_command(
        OUTPUT "${chat_proto_srcs}" "${chat_proto_hdrs}" "${chat_grpc_srcs}" "${chat_grpc_hdrs}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "../protos/gen/cpp"
        --cpp_out "../protos/gen/cpp"
        -I "${chat_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${chat_proto}"
        DEPENDS "${chat_proto}")

add_custom_command(
        OUTPUT "${dbaccessor_proto_srcs}" "${dbaccessor_proto_hdrs}" "${dbaccessor_grpc_srcs}" "${dbaccessor_grpc_hdrs}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "../protos/gen/cpp"
        --cpp_out "../protos/gen/cpp"
        -I "${dbaccessor_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${dbaccessor_proto}"
        DEPENDS "${dbaccessor_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}/protos/gen/cpp")


add_executable(chat_server main.cpp chat_server.cpp chat_dbaccessor.cpp ${chat_proto_srcs} ${chat_grpc_srcs} ${dbaccessor_proto_srcs} ${dbaccessor_grpc_srcs})
target_link_libraries(chat_server ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

