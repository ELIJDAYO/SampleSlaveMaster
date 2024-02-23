#include <iostream>
#include <WS2tcpip.h>
#include <vector>
#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_PORT "27016"   // Port to listen for incoming connections

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // Create a socket for the slave server to listen for connections
    SOCKET slaveSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (slaveSocket == INVALID_SOCKET) {
        std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Bind the socket to the specified port
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(27016); // Port to listen for incoming connections
    addr.sin_addr.s_addr = INADDR_ANY;

    result = bind(slaveSocket, (sockaddr*)&addr, sizeof(addr));
    if (result == SOCKET_ERROR) {
        std::cerr << "bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(slaveSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    result = listen(slaveSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        std::cerr << "listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(slaveSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Slave server listening on port " << DEFAULT_PORT << std::endl;

    // Vector to store received numbers
    std::vector<int> numbers;

    // Accept incoming connections and handle them
    SOCKET clientSocket;
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    while (true) {
        // Accept a client socket
        clientSocket = accept(slaveSocket, (sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
            closesocket(slaveSocket);
            WSACleanup();
            return 1;
        }

        // Receive the size of the vector
        int size;
        result = recv(clientSocket, reinterpret_cast<char*>(&size), sizeof(int), 0);
        if (result > 0) {
            std::cout << "Received vector size: " << size << std::endl;
        }
        else {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            continue; // Continue listening for more connections
        }

        // Receive the vector elements
        numbers.resize(size);
        for (int i = 0; i < size; ++i) {
            result = recv(clientSocket, reinterpret_cast<char*>(&numbers[i]), sizeof(int), 0);
            if (result > 0) {
                std::cout << "Received number: " << numbers[i] << " from master server." << std::endl;
            }
            else {
                std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
                closesocket(clientSocket);
                break; // Stop receiving numbers
            }
        }

        // Print the received vector
        std::cout << "Received vector:";
        for (int num : numbers) {
            std::cout << " " << num;
        }
        std::cout << std::endl;

        // Close the client socket
        closesocket(clientSocket);
    }

    // Close the slave server socket and cleanup
    closesocket(slaveSocket);
    WSACleanup();

    return 0;
}
