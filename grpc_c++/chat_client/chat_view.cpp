#include <iostream>

#include "chat_view.h"

using namespace std;

void ChatClientView::loginCheck() {
    loginView();

    string input_user_id;
    getline(cin, input_user_id);

    if(chatClient.getLoginSession()) {
        chatClient.reLogin(input_user_id);
        return;
    }

    chatClient.login(input_user_id);
    return;
}

void ChatClientView::loginView() {
    system("clear");
    cout << "=============================" << " 채팅 프로그램 " << "=============================" << endl << endl;
    cout << "이제부터 사용할 아이디를 입력해 주세요 : ";
}

void ChatClientView::userListView() {
    system("clear");
    cout << "===========================" << " 현재 접속중인 사용자 " << "===========================" << endl << endl;
    cout << "로그인한 사용자 목록입니다.";
    cout << "메뉴화면은 exit을 입력해 주세요." << endl << endl;
    cout << "=========================================================================" << endl;

    chatClient.userList();

    string input;
    while(1) {
        getline(cin, input);
        cout << input << endl;
        if(input == "exit") break;
    }

}

void ChatClientView::mainView() {
    system("clear");
    cout << "=============================" << " 채팅 프로그램 " << "=============================" << endl << endl;
    cout << "로그인을 하지 않고 다른 메뉴를 사용했을 때, 예외처리는 하지 않았습니다." << endl;
    cout << "로그인 후에 다른 메뉴를 사용해 주세요! " << endl << endl;
    cout << "메뉴는 숫자 한글자만 입력해 주시면 됩니다." << endl << endl;
    cout << "=========================================================================" << endl;
    cout << "**중요**" << endl;
    cout << "채팅시 유저의 아이디를 metadata로 서버에 전송하는데, " << endl;
    cout << "아이디가 한글일때 클라이언트가 종료됩니다. 이유를 찾고있습니다." << endl;
    cout << "따라서 아이디는 영문으로 부탁드립니다! (채팅은 한글로 가능합니다!)" << endl;
    cout << "=========================================================================" << endl << endl;
    cout << "사용자 아이디 : " << chatClient.getUser_id() << endl << endl;
    cout << "=========================================================================" << endl;
    if(chatClient.getLoginSession()){
        cout << "1. 다른 아이디로 로그인" << endl;
    } else {
        cout << "1. 로그인" << endl;
    }
    cout << "2. 사용자 목록 출력" << endl;
    cout << "3. 채팅방 입장" << endl;
    cout << "4. 종료" << endl;
}

void ChatClientView::chatRoomView() {
    cout << "=============================" << " 채팅방 입장 " << "=============================" << endl << endl;
    cout << "종료하시려면 exit을 입력해 주세요." << endl << endl;
    cout << "=========================================================================" << endl;

    chatClient.enterChatRoom();
}