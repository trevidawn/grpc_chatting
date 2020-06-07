#ifndef CHAT_CHAT_VIEW_H
#define CHAT_CHAT_VIEW_H

#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "chat_client.h"

class ChatClientView {
public:
    ChatClientView() : chatClient(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials())) {};

    void loginCheck();
    void loginView();
    void chatRoomListView();
    void createChatRoomView();
    void mainView();
    void chatRoomView();

private:
    ChatClient chatClient;
};

#endif //CHAT_CHAT_VIEW_H
