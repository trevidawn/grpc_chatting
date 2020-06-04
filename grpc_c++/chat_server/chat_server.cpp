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

    if(userIdList.find(user->user_id()) != userIdList.end()) {
        sr->set_responseflag(false);
        return grpc::Status::OK;
    }

    cout << user->user_id() << "님이 로그인 했습니다. " << endl;
    sr->set_responseflag(true);
    userIdList.insert(user->user_id());

    return grpc::Status::OK;
}

grpc::Status
ChatServiceImpl::userList(grpc::ServerContext *context, const cpp_chat::Empty* empty, grpc::ServerWriter <cpp_chat::User> *writer) {

    cpp_chat::User user;
    for (auto iter = userIdList.begin(); iter != userIdList.end(); ++iter) {
        user.set_user_id(*iter);
        writer->Write(user);
    }

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

    if(userIdList.find(new_user_id) != userIdList.end()) {
        sr->set_responseflag(false);
        return grpc::Status::OK;
    }

    userIdList.erase(userIdList.find(old_user_id));
    userIdList.insert(new_user_id);
    sr->set_responseflag(true);

    cout << old_user_id << "에서 " << new_user_id << "으로 재로그인 했습니다." << endl;

    return grpc::Status::OK;
}

grpc::Status
ChatServiceImpl::enterChatRoom(grpc::ServerContext *context,
                               grpc::ServerReaderWriter <cpp_chat::Message, cpp_chat::Message> *stream) {

    int n = v.size();
    cout << "===========================" << "채팅방 접속 " << "===========================" << endl;
    printMetadata(context);
    cout << "Time : " << getTimeStr() << endl;
    cout << "==================================================================" << endl;

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

                if(message.user_id() != "") {
                    userIdList.erase(userIdList.find(message.user_id()));
                }
                lk.unlock();
                break;
            }

            cout << message.user_id() << " : " << message.text() << endl;

            v.push_back(message);
            lk.unlock();
        }
    }, &endFlag);


    thread t2([=, &n, &stream](bool *endflag) {
        while (1) {
            if (*endflag) break;
            if (n < v.size()) {
                stream->Write(v[n]);
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
    return grpc::Status::OK;
}

void ChatServiceImpl::printMetadata(grpc::ServerContext *context) {
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
        }
        cout << endl;
    }
}

string ChatServiceImpl::getTimeStr(){
    time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());

    string s(30, '\0');
    strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return s;
}