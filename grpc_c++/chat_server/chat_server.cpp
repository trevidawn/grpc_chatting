#include <iostream>
#include <thread>
#include <vector>
#include <condition_variable>
#include <chrono>
#include <map>
#include <set>
#include <ctime>

//#include <grpc/grpc.h>
//#include <grpcpp/server.h>
//#include <grpcpp/server_context.h>
//#include <grpcpp/server_builder.h>
//#include <grpcpp/security/server_credentials.h>

#include "chat_server.h"

using namespace std;

grpc::Status
ChatServiceImpl::login(grpc::ServerContext *context, const cpp_chat::User *user, cpp_chat::SimpleResponse *sr) {

    if(user_room_mapper.find(user->user_id()) != user_room_mapper.end()) {
        sr->set_responseflag(false);
        return grpc::Status::OK;
    }

    cout << user->user_id() << "님이 로그인 했습니다. " << endl;
    sr->set_responseflag(true);

    user_room_mapper[user->user_id()] = "";
    return grpc::Status::OK;
}

grpc::Status
ChatServiceImpl::chatRoomList(grpc::ServerContext *context, const cpp_chat::Empty *empty, grpc::ServerWriter <cpp_chat::ChatRoom> *writer) {

    cpp_chat::ChatRoom chatRoom;
    for (auto iter = rooms.begin(); iter != rooms.end(); ++iter) {
        chatRoom.set_room_name(iter->first);
        writer->Write(chatRoom);
    }

    return grpc::Status::OK;
}

grpc::Status
ChatServiceImpl::createChatRoom(grpc::ServerContext *context, const cpp_chat::ChatRoom *chatRoom,
                            cpp_chat::SimpleResponse *sr) {
    Chat_Room chat_Room;
    chat_Room.room_name = chatRoom->room_name();

    if(rooms.find(chatRoom->room_name()) != rooms.end()) {
        sr->set_responseflag(false);
        return grpc::Status::OK;
    }

    rooms[chatRoom->room_name()] = chat_Room;
    sr->set_responseflag(true);

    cout << "===========================" << "채팅방 생성 " << "===========================" << endl;
    cout << "채팅방 이름 : " << chatRoom->room_name() << endl;
    cout << "총 채팅방 개수 : " << rooms.size() << endl;
    cout << "==================================================================" << endl;

    return grpc::Status::OK;
}

grpc::Status
ChatServiceImpl::reLogin(grpc::ServerContext *context, grpc::ServerReader<cpp_chat::User> *reader, cpp_chat::SimpleResponse *sr) {

    cpp_chat::User user;
    string old_user_id, new_user_id;

    reader->Read(&user);
    old_user_id = user.user_id();
    reader->Read(&user);
    new_user_id = user.user_id();

    if(user_room_mapper.find(new_user_id) != user_room_mapper.end()) {
        sr->set_responseflag(false);
        return grpc::Status::OK;
    }

    user_room_mapper.erase(old_user_id);
    user_room_mapper[new_user_id] = "";
    sr->set_responseflag(true);

    cout << old_user_id << "에서 " << new_user_id << "으로 재로그인 했습니다." << endl;

    return grpc::Status::OK;
}

grpc::Status
ChatServiceImpl::enterChatRoom(grpc::ServerContext *context, const cpp_chat::ChatRoom *chatRoom,
                               cpp_chat::SimpleResponse *sr) {

    if(rooms.find(chatRoom->room_name()) == rooms.end()) {
        sr->set_responseflag(false);
        return grpc::Status::OK;
    }

    user_room_mapper[chatRoom->user().user_id()] = chatRoom->room_name();
    sr->set_responseflag(true);
    return grpc::Status::OK;
}

grpc::Status
ChatServiceImpl::chatting(grpc::ServerContext *context,
                               grpc::ServerReaderWriter <cpp_chat::Message, cpp_chat::Message> *stream) {

    cout << "===========================" << "채팅방 접속 " << "===========================" << endl;
    string USER_ID = printMetadata(context);
    cout << "Time : " << getTimeStr() << endl;
    cout << "==================================================================" << endl;

    string ROOM_NAME = user_room_mapper[USER_ID];

    rooms[ROOM_NAME].userList.insert(USER_ID);
    int n = rooms[ROOM_NAME].messages.size();

    // 각각의 클라이언트마다 독립적으로 사용하는 mutex, condition_variable
    mutex read_m;
    condition_variable read_cv;
    bool endFlag = false;
    thread t([=, &read_m, &read_cv](bool *endflag) {
        cpp_chat::Message message;
        while (1) {
            unique_lock<mutex> lk(read_m);
            read_cv.wait(lk, [=, &stream, &message]() -> bool { return stream->Read(&message); });

            if (message.text() == "exit") {
                *endflag = true;
                message.set_text("exit");
                stream->Write(message);

                lk.unlock();
                break;
            }

            cout << message.user_id() << " : " << message.text() << endl;

            // DB insert using gRPC
            //dbAccessorClient.insertMessage(message.user_id(), message.text());

            rooms[ROOM_NAME].messages.push_back(message);
            lk.unlock();
        }
    }, &endFlag);


    thread t2([=, &n, &stream](bool *endflag) {
        while (1) {
            if (*endflag) break;
            if (n < rooms[ROOM_NAME].messages.size()) {
                stream->Write(rooms[ROOM_NAME].messages[n]);
                n++;
            }

            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }, &endFlag);

    t.join();
    t2.join();

    cout << "===========================" << " 채팅 종료 " << "===========================" << endl;
    printMetadata(context);
    cout << "Time : " << getTimeStr() << endl;
    cout << "==================================================================" << endl;
    rooms[ROOM_NAME].userList.erase(rooms[ROOM_NAME].userList.find(USER_ID));

    return grpc::Status::OK;
}

string ChatServiceImpl::printMetadata(grpc::ServerContext *context) {

    string ret;
    multimap<grpc::string_ref, grpc::string_ref> metadata = context->client_metadata();
    for (auto iter = metadata.begin(); iter != metadata.end(); ++iter) {
        cout << iter->first << " : ";

        size_t isbin = iter->first.find("-bin");
        if((isbin != std::string::npos) && (isbin + 4 == iter->first.size())) {
            cout << std::hex;
            for (auto c : iter->second) {
                cout << static_cast<unsigned int>(c);
            }
            cout << std::dec;
        } else {
            cout << iter->second;
            if(iter->first == "user_id") ret = iter->second.data();
        }
        cout << endl;
    }

    return ret;
}

string ChatServiceImpl::getTimeStr(){
    time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());

    string s(30, '\0');
    strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return s;
}