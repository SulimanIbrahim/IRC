#ifndef SERVER_HPP
#define SERVER_HPP

//colors 
#define RED "\033[31m"
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"



#include "Parser.hpp"
#include <fcntl.h>
#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

class Server {
private:
    bool _running;
    int _port;
    std::string _password;
    int _serverSocket;
    std::vector<int> _clientSockets;
    struct sockaddr_in _server_addr;
    Parser _parser;

public:
    Server(int ac, char **av);
    ~Server();

    void start();
    void setupSocket();
    void bindSocket();
    void listenSocket();
    void acceptClinets();
    void stop();
};

#endif
