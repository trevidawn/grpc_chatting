#ifndef CHAT_CHAT_SERVER_H
#define CHAT_CHAT_SERVER_H

#include "../protos/gen/chat.grpc.pb.h"

class ChatServiceImpl final : public cpp_chat::chat::Service {
public:
    grpc::Status
    login(grpc::ServerContext *context, const cpp_chat::User *user, cpp_chat::SimpleResponse *sr) override;

    grpc::Status
    userList(grpc::ServerContext *context, const cpp_chat::Empty *empty,
             grpc::ServerWriter<cpp_chat::User> *writer) override;

    grpc::Status
    reLogin(grpc::ServerContext *context, grpc::ServerReader<cpp_chat::User> *reader,
            cpp_chat::SimpleResponse *sr) override;

    grpc::Status
    enterChatRoom(grpc::ServerContext *context,
                  grpc::ServerReaderWriter<cpp_chat::Message, cpp_chat::Message> *stream) override;

    void printMetadata(grpc::ServerContext *context);
    std::string getTimeStr();

private:
    std::vector<cpp_chat::Message> v;
    std::set<std::string> userIdList;
};

#endif //CHAT_CHAT_SERVER_H
