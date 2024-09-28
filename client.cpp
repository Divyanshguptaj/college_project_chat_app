#include<iostream>
#include<ws2tcpip.h>
#include<tchar.h>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
/*
    1.Initilize the socket
    2.Create Socket
    3.Connect to Server
    4.Send /Recieve
    5.Close the socket
*/

bool Initilize(){
    WSADATA data;
    return WSAStartup(MAKEWORD(2,2),&data)==0;
}
int main(){
    cout << "Client Side Program "<<endl;

    // Step 1 - Initileze -
    if(!Initilize()){
        cout<< "Initilize Failed.."<<endl;
        return 1;
    }

    // Step 2 - Create Socket - 
    SOCKET s;
    s = socket(AF_INET, SOCK_STREAM,0);
    if(s==INVALID_SOCKET){
        cout<< "Invalid socket"<<endl;
        return 1;
    }

    // Step 3 - Connect to server -
    int port = 12345;
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port); 
    string serveraddrress = "127.0.0.1";
    //inet_pton is not working again so i am using its replacement - 
    // inet_pton(AF_INET,serveraddrress.c_str(),&(serveraddr.sin_addr));
    serveraddr.sin_addr.s_addr = inet_addr(serveraddrress.c_str());
    if (serveraddr.sin_addr.s_addr == INADDR_NONE) {
        cout << "Setting address structure failed" << endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }
    if(connect(s,reinterpret_cast<sockaddr*>(&serveraddr),sizeof(serveraddr))){
        cout<< "Server connection failed"<<endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }
    cout<< "Connection to server Succesfully . . ."<<endl;

    //Step 4 - Send and recieve data - 
    //sending -
    string msg = "Hello what's up !";
    int bytesent = send(s,msg.c_str(),msg.length(),0);
    if(bytesent==SOCKET_ERROR){
        cout<< "Can't send the message."<<endl;
        closesocket(s);
        WSACleanup(); 
    }

    // Step 5 - Close the socket - 
    closesocket(s);
    WSACleanup();
    return 0;
}