#ifndef INC_5_GRPC_CHAT_CHAT_DBACCESSOR_H
#define INC_5_GRPC_CHAT_CHAT_DBACCESSOR_H

#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "../protos/gen/cpp/dbaccessor.grpc.pb.h"

class DbAccessorClient {
public:
    DbAccessorClient(std::shared_ptr<grpc::Channel> channel) : stub_(dbaccessor::dbaccessor::NewStub(channel)) {};

    void insertMessage(std::string user_id, std::string text);

private:
    std::unique_ptr<dbaccessor::dbaccessor::Stub> stub_;
};

#endif //INC_5_GRPC_CHAT_CHAT_DBACCESSOR_H
