#ifndef COMMANDS_HPP
#define COMMANDS_HPP

// Forward declarations
class Server;
class Channel;
class Client;
class Commands; // Add forward declaration for Commands itself


#include <iostream>
#include <cstdlib>
#include <string>
#include <cctype>
#include <vector>

// Include Server.hpp after the forward declarations
#include "Server.hpp"

class Commands {
protected:
    Server* _server; // Add server reference
public:
    Commands(Server* server);
    virtual std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Clients) = 0;
    virtual ~Commands();
    static std::string Help();
    static std::string AuthMsg();
};

class Privmsg : public Commands {
    public:
        Privmsg(Server* server);
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};

class Topic : public Commands {
    public:
        Topic(Server* server);
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};


class Part : public Commands {
    public:
        Part(Server* server);
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Clients);
};

class Join : public Commands {
    public:
        Join(Server* server);
        std::string execute(Client &client, std::vector<std::string> &tokens,std::vector<Channel> &Channels, std::vector<Client> &Clients);
};

class Invite : public Commands {
    public:
        Invite(Server* server);
        std::string execute(Client &client, std::vector<std::string> &tokens,std::vector<Channel> &Channels, std::vector<Client> &Clients);
};

class Kick : public Commands {
    public:
        Kick(Server* server);
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};
class DCCSend : public Commands {
    public:
        DCCSend(Server* server);
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};

class DCCAccept : public Commands {
    public:
        DCCAccept(Server* server);
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};
// class Pubmsg : public Commands {
//     public:
//         Pubmsg(Server* server);
//         std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
// };

// class List : public Commands {
//     public:
//         List(Server* server);
//         std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
// };

class DCCReject : public Commands {
    public:
        DCCReject(Server* server);
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};
class Mode : public Commands {
    public:
        Mode(Server* server);
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};

class Cap : public Commands {
    public:
        Cap(Server* server);
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};
class Ping : public Commands {
    public:
        Ping(Server* server);
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};

class Quit : public Commands {
    public:
        Quit(Server* server);
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};

class Notice : public Commands {
    public:
        Notice(Server* server);
        std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients);
};

class BotCommand : public Commands {
public:
    BotCommand(Server* server);
    std::string execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Clients);
};

#endif