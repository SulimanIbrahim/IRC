#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>
#include <cstdlib>
#include <string>
#include <cctype>
#include <vector>
#include "Client.hpp"
#include "Channel.hpp"

class Commands {
public:
    virtual std::string execute(Client &client, std::string token1, std::string token2, std::vector<Channel> &Channels, std::vector<Client> &Clients) = 0;
    virtual ~Commands();
    static std::string Help();
    static std::string AuthMsg();
};

class Privmsg : public Commands {
    public:
        std::string execute(Client &client, std::string to, std::string message, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};

class Join : public Commands {
    public:
        std::string execute(Client &client, std::string channel_name, std::string ignore,std::vector<Channel> &Channels, std::vector<Client> &Clients);
};

class Kick : public Commands {
    public:
        std::string execute(Client &client, std::string username, std::string from_channel, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};






#endif
