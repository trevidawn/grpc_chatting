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

void ChatClientView::chatRoomListView() {
    system("clear");
    cout << "===========================" << " 채팅방 목록 " << "===========================" << endl << endl;
    cout << "메뉴화면으로 돌아가려면 exit을 입력해 주세요." << endl << endl;
    cout << "=========================================================================" << endl;

    chatClient.chatRoomList();

    string input;
    while(1) {
        getline(cin, input);
        cout << input << endl;
        if(input == "exit") break;
    }

}

void ChatClientView::createChatRoomView() {
    cout << "=============================" << " 채팅방 생성 " << "=============================" << endl;
    cout << "채팅방 이름을 입력해 주세요 : ";

    string chatRoomName;
    getline(cin, chatRoomName);

    chatClient.createChatRoom(chatRoomName);
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
    cout << "한글을 파싱하는 과정에서 에러가 발생하여 클라이언트가 종료됩니다.." << endl;
    cout << "따라서 아이디는 영문으로 부탁드립니다! (채팅방 이름 및 채팅은 한글로 가능합니다!)" << endl;
    cout << "=========================================================================" << endl << endl;
    cout << "사용자 아이디 : " << chatClient.getUser_id() << endl << endl;
    cout << "=========================================================================" << endl;
    if(chatClient.getLoginSession()){
        cout << "1. 다른 아이디로 로그인" << endl;
    } else {
        cout << "1. 로그인" << endl;
    }
    cout << "2. 채팅방 목록 출력" << endl;
    cout << "3. 채팅방 생성" << endl;
    cout << "4. 채팅방 입장" << endl;
    cout << "5. 종료" << endl;
}

void ChatClientView::chatRoomView() {
    cout << "=============================" << " 채팅방 입장 " << "=============================" << endl << endl;
    cout << "종료하시려면 exit을 입력해 주세요." << endl << endl;
    cout << "=========================================================================" << endl;

    cout << "채팅방 이름을 입력해 주세요 : ";
    string chatRoomName;
    getline(cin,chatRoomName);

    system("clear");
    cout << "=============================" << " 채팅방 " << "=============================" << endl << endl;
    cout << "채팅방 이름 : " << chatRoomName << endl << endl;
    cout << "종료하시려면 exit을 입력해 주세요." << endl << endl;
    cout << "=========================================================================" << endl;

    chatClient.enterChatRoom(chatRoomName);
}