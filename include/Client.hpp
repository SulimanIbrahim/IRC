#ifndef CLIENT_HPP
#define CLIENT_HPP

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
#include <poll.h>

class Client {
private:
    pollfd _pfd;
    std::string _username;
    std::string _nickname;

public:
    Client(int _clientSockets);
    ~Client();

    void set_nick(std::string nickname);
    void set_username(std::string username);
    int get_socket();
    pollfd get_pfd();
    std::string get_nick();
    std::string get_username();
};

#endif
