#include "chat_dbaccessor.h"

using namespace std;

void DbAccessorClient::insertMessage(string user_id, string text) {

    grpc::ClientContext context;
    dbaccessor::Message msg;
    dbaccessor::SimpleResponse sr;

    msg.set_user_id(user_id);
    msg.set_text(text);

    grpc::Status status = stub_->insertMessage(&context, msg, &sr);

    if(!status.ok()) {
        cout << "insertMessage rpc 실패" << endl;
    }

    return;
}