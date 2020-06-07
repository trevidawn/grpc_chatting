#ifndef CHAT_CHAT_SERVER_H
#define CHAT_CHAT_SERVER_H

#include "../protos/gen/cpp/chat.grpc.pb.h"
#include "chat_dbaccessor.h"

typedef struct chat_Room {
    std::string room_name;
    std::vector<cpp_chat::Message> messages;
    std::set<std::string> userList;
} Chat_Room;


class ChatServiceImpl final : public cpp_chat::chat::Service {
public:
    ChatServiceImpl() : dbAccessorClient(grpc::CreateChannel("localhost:50052", grpc::InsecureChannelCredentials())) {};

    grpc::Status
    login(grpc::ServerContext *context, const cpp_chat::User *user, cpp_chat::SimpleResponse *sr) override;

    grpc::Status
    reLogin(grpc::ServerContext *context, grpc::ServerReader<cpp_chat::User> *reader, cpp_chat::SimpleResponse *sr) override;

    grpc::Status
    chatRoomList(grpc::ServerContext *context, const cpp_chat::Empty *empty, grpc::ServerWriter<cpp_chat::ChatRoom> *writer) override;

    grpc::Status
    createChatRoom(grpc::ServerContext *context, const cpp_chat::ChatRoom *chatRoom, cpp_chat::SimpleResponse *sr) override;

    grpc::Status
    enterChatRoom(grpc::ServerContext *context, const cpp_chat::ChatRoom *chatRoom, cpp_chat::SimpleResponse *sr) override;

    grpc::Status
    chatting(grpc::ServerContext *context, grpc::ServerReaderWriter<cpp_chat::Message, cpp_chat::Message> *stream) override;

    std::string printMetadata(grpc::ServerContext *context);
    std::string getTimeStr();

private:
    std::map<std::string, Chat_Room> rooms;
    std::map<std::string, std::string> user_room_mapper;
    //std::vector<cpp_chat::Message> v;
    //std::set<std::string> userIdList;

    DbAccessorClient dbAccessorClient;
};

#endif //CHAT_CHAT_SERVER_H
