#include <iostream>

#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_context.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/security/server_credentials.h>

#include "chat_server.h"

using namespace std;

void RunServer() {
    string server_address("localhost:50051");
    ChatServiceImpl service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    unique_ptr<grpc::Server> server(builder.BuildAndStart());
    cout << "Server listening on " << server_address << endl;

    server->Wait();
}

int main(int argc, char **argv) {
    RunServer();

    return 0;
}
