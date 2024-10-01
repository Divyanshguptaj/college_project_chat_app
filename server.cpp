#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;

// Function to send and receive messages
void interactWithClient(int clientSocket, vector<int>& clients) {
    cout << "Client connected" << endl;
    char buffer[4096];

    while (true) {
        int dataRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (dataRcv <= 0) {
            cout << "Connection lost with a client" << endl;
            break;
        }

        string message(buffer, dataRcv);
        cout << "Message received: " << message << endl;

        for (auto client : clients) {
            if (client != clientSocket) {
                send(client, message.c_str(), message.length(), 0);
            }
        }
    }

    auto findClient = find(clients.begin(), clients.end(), clientSocket);
    if (findClient != clients.end()) {
        clients.erase(findClient);
    }
    close(clientSocket);
}

// Main function
int main() {
    cout << "Server program started" << endl;

    // Step 1: Create socket
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == -1) {
        cout << "Socket creation failed" << endl;
        return 1;
    }

    // Step 2: Get IP and port
    sockaddr_in serverAddr;
    int portValue = 12345;
    serverAddr.sin_port = htons(portValue);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Use INADDR_ANY to accept connections from any IP

    // Step 3: Bind the socket
    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        cout << "Binding failed" << endl;
        close(listenSocket);
        return 1;
    }

    // Step 4: Listen on the socket
    if (listen(listenSocket, SOMAXCONN) == -1) {
        cout << "Error listening" << endl;
        close(listenSocket);
        return 1;
    }

    cout << "Server has started listening on port: " << portValue << endl;

    // Step 5: Accept clients -
    vector<int> clients;
    while (true) {
        int clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            cout << "Invalid client socket" << endl;
            close(listenSocket);
            return 1;
        }
        cout << "Connected to client successfully" << endl;

        clients.push_back(clientSocket);
        thread t1(interactWithClient, clientSocket, ref(clients));
        t1.detach(); // Detach the thread to handle clients independently
    }

    // Step 6: Close the listening socket
    close(listenSocket);

    cout << "Server program ended." << endl;
    return 0;
}
