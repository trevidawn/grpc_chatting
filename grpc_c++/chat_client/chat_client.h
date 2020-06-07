#ifndef CHAT_CHAT_CLIENT_H
#define CHAT_CHAT_CLIENT_H

#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "../protos/gen/cpp/chat.grpc.pb.h"

class ChatClient {
public:
    ChatClient(std::shared_ptr<grpc::Channel> channel) : stub_(cpp_chat::chat::NewStub(channel)) {};

    //grpc
    void login(const std::string input_user_id);
    void reLogin(const std::string input_user_id);
    void userList();
    void enterChatRoom();

    //getter
    bool getLoginSession();
    std::string getUser_id();

private:
    std::unique_ptr<cpp_chat::chat::Stub> stub_;
    bool loginSession;
    std::string user_id;
    //string input;
};

#endif //CHAT_CHAT_CLIENT_H