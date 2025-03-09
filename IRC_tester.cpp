#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

class IRCClient {
public:
    IRCClient();
    IRCClient(const std::string& host, int port);
    ~IRCClient();
    
    void connectToServer();
    void sendMessage(const std::string& message);
    std::string receiveMessage();
    
private:
    int _clientSocket;
    sockaddr_in _serverAddr;
    std::string _host;
    int _port;
};

IRCClient::IRCClient(){

}

IRCClient::IRCClient(const std::string& host, int port)
    : _host(host), _port(port), _clientSocket(-1) {
    _clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_clientSocket < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        exit(1);
    }
}

IRCClient::~IRCClient() {
    std::cout << "/33[1;31mClient destructor has been called/33[0m" << std::endl;
    // if (_clientSocket != -1) {
    //     close(_clientSocket);
    // }
}

void IRCClient::connectToServer() {
    std::cout << "Connecting to: " << _host << " on port " << _port << std::endl;

    memset(&_serverAddr, 0, sizeof(_serverAddr));
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_port = htons(_port);

    int ptonResult = inet_pton(AF_INET, _host.c_str(), &_serverAddr.sin_addr);
    if (ptonResult <= 0) {
        std::cerr << "Invalid address: " << _host << " (inet_pton failed with code " << ptonResult << ")" << std::endl;
        perror("inet_pton");  // Print detailed system error
        exit(1);
    }

    if (connect(_clientSocket, (struct sockaddr*)&_serverAddr, sizeof(_serverAddr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        perror("connect");
        exit(1);
    }

    std::cout << "Connected to server!" << std::endl;
}


std::string IRCClient::receiveMessage() {
    char buffer[1024] = {0};
    int bytesReceived = recv(_clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived < 0) {
        std::cerr << "Failed to receive message" << std::endl;
    } else if (bytesReceived == 0) {
        return "Connection closed";
    }
    return std::string(buffer, bytesReceived);
}

int main() {
    std::string serverAddress = "127.0.0.1";
    int port = 6667;

   IRCClient *clients = new IRCClient[1010];
    for (int i = 0; i < 1010; i++)
    {
        clients[i] = IRCClient(serverAddress, port);
        clients[i].connectToServer();
    }

    while(1)
        ;

    delete[] clients; 



    
    
    // Connect to the server
    
    // Receive and print the banner from the server
    // std::cout << "Received: " << client.receiveMessage() << std::endl;

    // // Send authentication command (assuming it's required)
    // client.sendMessage("PASS my_password\r\n");
    // std::cout << "Received: " << client.receiveMessage() << std::endl;

    // client.sendMessage("NICK test_user\r\n");
    // std::cout << "Received: " << client.receiveMessage() << std::endl;

    // client.sendMessage("USER test_user 0 * :Test User\r\n");
    // std::cout << "Received: " << client.receiveMessage() << std::endl;

    // // Test sending other commands
    // client.sendMessage("JOIN #test_channel\r\n");
    // std::cout << "Received: " << client.receiveMessage() << std::endl;

    // client.sendMessage("PRIVMSG #test_channel :Hello, World!\r\n");
    // std::cout << "Received: " << client.receiveMessage() << std::endl;

    // // You can continue sending other commands and checking responses
    // client.sendMessage("LIST\r\n");
    // std::cout << "Received: " << client.receiveMessage() << std::endl;

    // // Close the connection when done
    // client.sendMessage("QUIT\r\n");
    // std::cout << "Received: " << client.receiveMessage() << std::endl;

    return 0;
}
