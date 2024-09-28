#include<iostream>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<tchar.h>
#include <thread>
#include<vector>
#include<algorithm>
using namespace std;

//initlize of winsock library -
#pragma comment(lib,"ws2_32.lib");
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600  // Define the appropriate version
#endif
/*
    1.intilize winsock library 
    2.create socket
    3.get ip and port 
    4.bind the ip/port with the socket
    5.listen on the socket
    6.accept
    7.recieve and send
    8.close the socket
    9.clean the winsock
*/

//function to initilize a socket - 
bool Initialize(){
    WSADATA data;
    return WSAStartup(MAKEWORD(2,2),&data)==0;
}

//function to send and recieve message - 
void interactWithClient(SOCKET clientsocket,vector<SOCKET>&clients){
    cout<< "client connected"<<endl;
    char buffer[4096];
    while(1){
        int data_rcv = recv(clientsocket,buffer,sizeof(buffer),0);
        if(data_rcv<=0){
            cout<< "Connection Lost with another client"<<endl;
            break;
        }
        string message(buffer,data_rcv);
        cout<< "Message Recieved : "<<message<<endl;
        for(auto client : clients){
            if(client != clientsocket)
                send(client,message.c_str(),message.length(),0);
        }
    }
    auto find_client = find(clients.begin(),clients.end(),clientsocket);
    if(find_client != clients.end()){
        clients.erase(find_client);
    }
    closesocket(clientsocket);
    return;
}

//main function -
int main(){
    cout<< "Server Program started"<<endl;
    //Step 1 - intilize the library - 
    if(!Initialize()) {
        cout<< "Win library intialization failed. . ."<<endl;
        return 1;    
    }

    //Step 2 - Create Socket - 
    SOCKET listenSocket = socket(AF_INET,SOCK_STREAM,0);
    if(listenSocket==INVALID_SOCKET){
        cout<<"socket creation failed"<<endl;
        return 1;
    }

    //Step 3 - Get ip and port - 
    sockaddr_in serveraddr;
    int port_value = 12345;
    serveraddr.sin_port = htons(port_value); //htonl - host to network lock converts integer to network integer.
    serveraddr.sin_family = AF_INET;
    //convert the ipaddress(0.0.0.0) put it inside the sin_family in binary form requires a header file also WS2tcpip.h & tchar.h
    // if (InetPton(AF_INET, "0.0.0.0", &serveraddr.sin_addr) != 1){
    //     cout<< "Setting address structure failed"<<endl;
    //     closesocket(listenSocket);
    //     WSACleanup();
    //     return 1;
    // }
    //InetPton is not working so this is the replacement for that -
    serveraddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    if (serveraddr.sin_addr.s_addr == INADDR_NONE) {
        cout << "Setting address structure failed" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Step 4 - Bind the socket -
    if(bind(listenSocket,reinterpret_cast<sockaddr*>(&serveraddr),sizeof(serveraddr))==SOCKET_ERROR){
        cout<< "Binding failed . . ."<<endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    //Step 5 - Listen socket - 
    if(listen(listenSocket,SOMAXCONN)==SOCKET_ERROR){
        cout<< "Error Listening"<<endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    cout<< "Server has started listing in port :"<< port_value <<endl;

    // Step 6 - Accept -
    //vector to save all the client that are connected -
    vector<SOCKET> clients;
    while(1){
        SOCKET clientsocket = accept(listenSocket,nullptr,nullptr);
        if(clientsocket==INVALID_SOCKET){
            cout<< "Invalid client socket"<<endl;
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }
        cout<< "Connected to client succuessfully . . ."<<endl;
        // Step 7 - Receive and Send - 
        // calling function to send and recieve and using threads to connect different clients parallel -
        // calling function to send and receive using threads to connect different clients in parallel
        clients.push_back(clientsocket);
        thread t1(interactWithClient, clientsocket, ref(clients));
        // t1.detach();
    }

    // Step 8 - Close the socket -
    closesocket(listenSocket);

    //Step 9 - to clean the winsock -
    cout<< "Server program end."<<endl;
    WSACleanup();

    return 0;
}