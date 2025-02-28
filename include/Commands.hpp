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
    virtual std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Clients) = 0;
    virtual ~Commands();
    static std::string Help();
    static std::string AuthMsg();
};

class Privmsg : public Commands {
    public:
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};

class Topic : public Commands {
    public:
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};


class Leave : public Commands {
    public:
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Clients);
};

class Join : public Commands {
    public:
        std::string execute(Client &client, std::vector<std::string> &tokens,std::vector<Channel> &Channels, std::vector<Client> &Clients);
};

class Invite : public Commands {
    public:
        std::string execute(Client &client, std::vector<std::string> &tokens,std::vector<Channel> &Channels, std::vector<Client> &Clients);
};

class Kick : public Commands {
    public:
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};
class DCCSend : public Commands {
    public:
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};

class DCCAccept : public Commands {
    public:
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};
class Pubmsg : public Commands {
    public:
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};

class List : public Commands {
    public:
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};

class DCCReject : public Commands {
    public:
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};
class Mode : public Commands {
    public:
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};

class Cap : public Commands {
    public:
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};
class Ping : public Commands {
    public:
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};

class Quit : public Commands {
    public:
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};

class Notice : public Commands {
    public:
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};

#endif