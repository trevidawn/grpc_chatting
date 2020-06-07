#include <iostream>
#include <thread>
#include <condition_variable>
#include <stdlib.h>

#include <grpc/grpc.h>
#include <grpcpp/client_context.h>
//#include <grpcpp/channel.h>
//#include <grpcpp/create_channel.h>
//#include <grpcpp/security/credentials.h>

#include "chat_client.h"

using namespace std;

// 터미널에서 입력한 한 줄 지워주는 함수
void upLinePrompt(int count) {
    for (int i = 0; i < count; ++i) {
        std::cout << "\33[2K"; //line clear
        std::cout << "\x1b[A"; //up line (ESC [ A) must be support VT100 escape seq
    }
}

/*
 * gRPC Method
 */
void ChatClient::login(const string input_user_id) {
    grpc::ClientContext context;
    cpp_chat::SimpleResponse sr;
    cpp_chat::User user;

    user.set_user_id(input_user_id);

    grpc::Status status = stub_->login(&context, user, &sr);
    if(!status.ok()) {
        cout << "login rpc 실패. " << endl;
        this_thread::sleep_for(chrono::milliseconds(700));
        return;
    }

    if (!sr.responseflag()){
        cout << "중복된 아이디 입니다." << endl;
        this_thread::sleep_for(chrono::milliseconds(700));
        return;
    }

    cout << "로그인 성공!" << endl;
    this->user_id = input_user_id;
    loginSession = true;
    this_thread::sleep_for(chrono::milliseconds(700));
    return;
}

void ChatClient::reLogin(const string input_user_id) {
    grpc::ClientContext context;
    cpp_chat::SimpleResponse sr;
    cpp_chat::User user;
    user.set_user_id(input_user_id);

    unique_ptr<grpc::ClientWriter<cpp_chat::User> > writer(stub_->reLogin(&context, &sr));

    user.set_user_id(this->user_id);
    writer->Write(user);
    user.set_user_id(input_user_id);
    writer->Write(user);

    writer->WritesDone();
    grpc::Status status = writer->Finish();

    if(!status.ok()) {
        cout << "reLogin grpc 실패." << endl;
        this_thread::sleep_for(chrono::milliseconds(700));
        return;
    }

    if(!sr.responseflag()) {
        cout << "중복된 아이디 입니다." << endl;
        this_thread::sleep_for(chrono::milliseconds(700));
        return;
    }

    cout << "재 로그인 성공!" << endl;
    this->user_id = input_user_id;
    this_thread::sleep_for(chrono::milliseconds(700));
    return;
}

void ChatClient::chatRoomList() {
    grpc::ClientContext context;
    cpp_chat::Empty empty;
    cpp_chat::ChatRoom chatRoom;

    unique_ptr<grpc::ClientReader<cpp_chat::ChatRoom> > reader(stub_->chatRoomList(&context, empty));

    while(reader->Read(&chatRoom)) {
        cout << chatRoom.room_name() << endl;
    }
    cout << endl;

    grpc::Status status = reader->Finish();
    if(!status.ok()) {
        cout << "userList rpc failed. " << endl;
    }
}

void ChatClient::createChatRoom(string chatRoomName) {
    grpc::ClientContext context;
    cpp_chat::ChatRoom chatRoom;
    cpp_chat::SimpleResponse sr;

    chatRoom.set_room_name(chatRoomName);

    grpc::Status status = stub_->createChatRoom(&context, chatRoom, &sr);
    if(!status.ok()) {
        cout << "createChatRoom rpc failed" << endl;
        this_thread::sleep_for(chrono::milliseconds(700));
        return;
    }

    if(!sr.responseflag()) {
        cout << "같은 이름의 채팅방이 존재합니다." << endl;
        this_thread::sleep_for(chrono::milliseconds(700));
        return;
    }

    cout << "채팅방 생성 성공!" << endl;
    this_thread::sleep_for(chrono::milliseconds(700));
    return;
}

void ChatClient::enterChatRoom(std::string chatRoomName) {
    grpc::ClientContext context;

    cpp_chat::ChatRoom chatRoom;
    cpp_chat::SimpleResponse sr;
    chatRoom.mutable_user()->set_user_id(user_id);
    chatRoom.set_room_name(chatRoomName);

    grpc::Status status;
    status = stub_->enterChatRoom(&context, chatRoom, &sr);
    if(!status.ok()) {
        cout << "enterChatRoom rpc failed" << endl;
        this_thread::sleep_for(chrono::milliseconds(700));
        return;
    }

    if(!sr.responseflag()) {
        cout << "채팅방이 존재하지 않습니다." << endl;
        this_thread::sleep_for(chrono::milliseconds(700));
        return;
    }

    chatting();
}

void ChatClient::chatting() {
    grpc::ClientContext context;

    context.AddMetadata("user_id", user_id);
    shared_ptr<grpc::ClientReaderWriter < cpp_chat::Message, cpp_chat::Message> > stream(
            stub_->chatting(&context));

    cpp_chat::Message message;
    mutex m;
    condition_variable cv;
    thread t([&m, &stream, &cv](cpp_chat::Message msg) {
        while (1) {
            unique_lock<mutex> lk(m);
            cv.wait(lk, [&stream, &msg]() -> bool { return stream->Read(&msg);});
            if(msg.text() == "exit") {
                lk.unlock();
                break;
            }
            cout << msg.user_id() << " : " << msg.text() << endl;

            lk.unlock();
        }
    }, message);

    string input;
    while(1) {
        getline(cin, input);

        upLinePrompt(1);

        message.set_user_id(user_id);
        message.set_text(input);
        stream->Write(message);

        if(input == "exit") {
            break;
        }
    }

    t.join();

    grpc::Status status = stream->Finish();
    if (!status.ok()) {
        cout << "rpc failed" << endl;
    }
};

/*
 * Getter
 */

bool ChatClient::getLoginSession() {
    return loginSession;
}

string ChatClient::getUser_id() {
    return user_id;
}