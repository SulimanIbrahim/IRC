#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Parser.hpp"
#include "Bot.hpp"
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
#include <map>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>

class Client {
    private:
        bool password_entered;
        bool registered;
        int _fd;
        std::string _ip;
        std::string _username;
        std::string _realname;
        std::string _nickname;
        std::string _hostname;
        std::vector<std::string> channels;
        Bot _bot;
        std::string _outBuffer;

    public:
        Client(int _clientSockets, std::string ip);
        Client(const Client &client);
        ~Client();

        void set_nick(std::string nickname);
        void set_username(std::string username);
        void set_realname(std::string realname);
        void set_hostname(std::string hostname);
        void setPasswordEntered(bool value);
        void addChannel(std::string channel);
        void removeChannel(std::string channel);
        bool isAuthentificated();
        int get_fd();
        std::string get_ip();
        std::string get_nick();
        std::string get_username();
        std::string get_realname();
        std::string get_hostname();
        bool is_inChannel(std::string channel);
        bool isPasswordEntered();
        bool isRegistered();
        void setRegistered(bool value);
        
        // Bot related methods
        void addActivity(const std::string& activity);
        std::string getBotResponse(std::string channel);

        void addToOutBuffer(const std::string& msg);
        std::string& getOutBuffer();
};


#endif
