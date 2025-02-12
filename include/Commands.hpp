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
    Commands();
    ~Commands();

    static std::string Pass(Client &client, const std::vector<std::string>& tokens, std::string password);
    static std::string Nick(Client &client, const std::vector<std::string>& tokens);
    static std::string User(Client &client, const std::vector<std::string>& tokens);
    static std::string Privmsg(Client &client, std::string to, std::string message, std::vector<Channel> &Channels);
    static std::string Join(Client &client, std::string channel, std::vector<Channel> &Channels, std::vector<Client> &Clients);
    static std::string Kick(Client &client, std::string username, std::string from_channel, std::vector<Channel> &Channels);
    static std::string Help();
    static std::string Auth();
};

#endif
