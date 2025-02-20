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
        bool password_entered;
        int _fd;
        std::string _username;
        std::string _nickname;
        std::string at_channel;

    public:
        Client(int _clientSockets);
        Client(const Client &client);
        ~Client();

        void set_nick(std::string nickname);
        void set_username(std::string username);
        void setPasswordEntered(bool value);
        void setChannel(std::string channel);
        std::string getChannel();
        int get_fd();
        std::string get_nick();
        std::string get_username();
        bool isPasswordEntered();
};

#endif
