#ifndef CHANNEL_HPP
#define CHANNEL_HPP

// Add this forward declaration at the top of the file
class Server;

#include "Server.hpp"
#include "Parser.hpp"
#include "Client.hpp"
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
#include <cstdio>
#include <sstream>


class Channel {
private:
    // Utility functions for string conversion (C++98 compatible replacement for std::to_string)
    std::string numberToString(int num) {
        std::ostringstream oss;
        oss << num;
        return oss.str();
    }
    
    std::string numberToString(unsigned long num) {
        std::ostringstream oss;
        oss << num;
        return oss.str();
    }
    const std::string _ChannelName;
    std::vector<Client> _Clients;
    std::vector<Client> _operators;
    std::string _Topic;
    std::string _Password;
    bool InviteOnly;
    bool TopicRistercted;
    bool PasswordProtected;
    Server* _server; // Add server pointer

public:
    Channel(Client client, std::string ChannelName, Server* server);
    Channel(Client client, std::string ChannelName, std::string password, Server* server);
    ~Channel();

    std::string listClients();
    bool isClientInChannel(Client &client);
    bool isClientInChannel(std::string &client);
    bool isOperator(Client &client);
    std::string getChannelName();
    std::string JoinChannel(Client client, std::vector<Client> &Clients);
    std::string Invite(Client client, std::string clien_to_add, std::vector<Client> &Clients);
    std::string kickClient(Client client, std::string clien_to_kick);
    void sendMessage(std::string message, Client &client, std::vector<Client> &Clients);
    std::string sendPrivateMessage(std::string message, Client &client, std::string to);
    std::string sendDCCRequest(std::string filename, Client &client, std::string to);
    std::string showTopic();
    void setTopic(std::string topic);
    std::string setInviteOnly_status(std::vector<std::string> tokens);
    std::string setTopicRisterction_status(std::vector<std::string> tokens);
    std::string setPrivate_status(std::vector<std::string> tokens);
    std::string setOperator_status(std::vector<std::string> tokens);
    void setPassword(std::string password);
    std::string leaveChannel(Client &client);
    std::string getPassword();
    bool isPrivate();
    bool isInviteOnly();
    bool isTopicRisterction();
    std::string mode(Client client, std::vector<std::string> tokens);
    std::string status();
    std::vector<Client> getClients();
};

#endif
