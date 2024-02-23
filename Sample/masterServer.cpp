#include <iostream>
#include <WS2tcpip.h>
#include <vector>
#include <chrono> // For delay
#include <thread> // For delay
#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_PORT "27015"   // Port to connect to slave server

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // Create a socket for the master server to connect to the slave server
    SOCKET masterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (masterSocket == INVALID_SOCKET) {
        std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Specify the address and port to connect to (slave server)
    sockaddr_in slaveAddr;
    slaveAddr.sin_family = AF_INET;
    slaveAddr.sin_port = htons(27016); // Slave server port
    inet_pton(AF_INET, "127.0.0.1", &slaveAddr.sin_addr); // Slave server IP address

    // Connect to the slave server
    result = connect(masterSocket, (sockaddr*)&slaveAddr, sizeof(slaveAddr));
    if (result == SOCKET_ERROR) {
        std::cerr << "connect failed with error: " << WSAGetLastError() << std::endl;
        closesocket(masterSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to slave server." << std::endl;

    // Define a vector to store numbers
    std::vector<int> numbers = { 1, 2, 3, 4, 5 };

    // Send the size of the vector
    int size = numbers.size();
    result = send(masterSocket, reinterpret_cast<char*>(&size), sizeof(int), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
        closesocket(masterSocket);
        WSACleanup();
        return 1;
    }

    // Send the vector elements
    for (int num : numbers) {
        result = send(masterSocket, reinterpret_cast<char*>(&num), sizeof(int), 0);
        if (result == SOCKET_ERROR) {
            std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
            closesocket(masterSocket);
            WSACleanup();
            return 1;
        }

        // Add a delay between sending each number
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 100 milliseconds delay
    }

    // Close the socket and cleanup
    closesocket(masterSocket);
    WSACleanup();

    return 0;
}
