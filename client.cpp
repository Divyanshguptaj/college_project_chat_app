#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include<thread>
using namespace std;

/*
    1. Initialize the socket
    2. Create Socket
    3. Connect to Server
    4. Send / Receive
    5. Close the socket
*/

bool Initialize() {
    // No explicit initialization needed in Linux
    return true;
}

void sendMessage(SOCKET s){
    string name;
    cout<< "Enter your chat name :"<<endl;
    cin>> name;
    string msg;
    while(1){
        getline(cin,msg);
        if(msg=="quit") break;
        string msg = name + " : " + msg;
        int bytesent = send(s,msg.c_str(),msg.length(),0);
        if(bytesent<=0){
            cout<< "Error sending message"<<endl;
            break;
        }
    }
    closesocket();
    WSAcleanup()
    return;
}

void rcvMessage(SOCKET s){
    int buffer[4096];
    int rcvlength;
    while(1){
        rcvlength = recv(s,buffer,sizeof(buffer),0);
        if(rcvlength<=0){
            cout<< "Disconnected to server"<<endl;
            break;
        }
        else{
            string msg = string(buffer,rcvlength);
            cout<<msg<<endl;
        }
    }
    closesocket();
    WSAcleanup();
    return;
}

int main() {
    cout << "Client Side Program" << endl;

    // Step 1 - Initialize
    if (!Initialize()) {
        cout << "Initialize Failed.." << endl;
        return 1;
    }

    // Step 2 - Create Socket
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        cout << "Invalid socket" << endl;
        return 1;
    }

    // Step 3 - Connect to server
    int port = 12345;
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    string serverAddress = "127.0.0.1";

    serveraddr.sin_addr.s_addr = inet_addr(serverAddress.c_str());
    if (serveraddr.sin_addr.s_addr == INADDR_NONE) {
        cout << "Setting address structure failed" << endl;
        close(s);
        return 1;
    }

    if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == -1) {
        cout << "Server connection failed" << endl;
        close(s);
        return 1;
    }
    cout << "Connection to server successfully..." << endl;

    // Step 4 - Send and receive data
    thread senderThread(sendMessage,s);
    thread rcvThread(rcvMessage,s);
    senderThread.join();
    rcvThread.join();
    // Sending
    //now we are doing this using thread - 
    // string msg = "Hello, what's up!";
    // int bytesSent = send(s, msg.c_str(), msg.length(), 0);
    // if (bytesSent == -1) {
    //     cout << "Can't send the message." << endl;
    //     close(s);
    //     return 1;
    // }

    // Optionally, you can add receiving logic here...

    // Step 5 - Close the socket
    // closesocket(s);
    return 0;
}
