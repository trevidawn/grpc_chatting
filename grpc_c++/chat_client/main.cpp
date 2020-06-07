#include <iostream>

#include "chat_view.h"

using namespace std;

int main(int argc, char **argv) {
    ChatClientView chatView;

    int input;
    bool flag = true;
    while(flag) {
        chatView.mainView();
        cout << "> ";
        cin >> input;
        cin.ignore(255, '\n');

        switch(input) {
            case 1:
                system("clear");
                chatView.loginCheck();
                break;
            case 2:
                system("clear");
                chatView.chatRoomListView();
                break;
            case 3:
                system("clear");
                chatView.createChatRoomView();
                break;
            case 4:
                system("clear");
                chatView.chatRoomView();
                break;
            case 5:
                cout << "채팅 프로그램 종료" << endl;
                flag = false;
                break;
            default:
                break;
        }
    }

    return 0;
}