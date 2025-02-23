#ifndef CHANNEL_HPP
#define CHANNEL_HPP

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

class Channel {
private:
    const std::string _ChannelName;
    std::vector<Client> _Clients;
    std::vector<Client> _operators;
    std::string _Topic;
    std::string _Password;
    bool i;
    bool t;
    bool k;

public:
    Channel(Client client, std::string ChannelName);
    Channel(Client client, std::string ChannelName, std::string password);
    ~Channel();

    std::string listClients();
    std::string getChannelName();
    std::string JoinChannel(Client client, std::vector<Client> &Clients);
    std::string Invite(Client client, std::string clien_to_add, std::vector<Client> &Clients);
    std::string kickClient(Client client, std::string clien_to_kick);
    void sendMessage(std::string message, Client &client);
    std::string sendPrivateMessage(std::string message, Client &client, std::string to);
    std::string showTopic(Client client);
    void setTopic(std::string topic);
    std::string setInviteOnly_status(std::vector<std::string> tokens);
    std::string setTopicRisterction_status(std::vector<std::string> tokens);
    std::string setPrivate_status(std::vector<std::string> tokens);
    std::string setOperator_status(std::vector<std::string> tokens);
    void setPassword(std::string password);
    std::string getPassword();
    bool isPrivate();
    bool isInviteOnly();
    bool isTopicRisterction();
    std::string mode(Client client, std::vector<std::string> tokens);
    std::string status();
    std::vector<Client> getClients();
};

#endif
