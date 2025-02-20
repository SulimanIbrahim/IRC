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
    bool _isPrivate;

public:
    Channel(Client client, std::string ChannelName);
    Channel(Client client, std::string ChannelName, std::string password);
    ~Channel();

    std::string listClients();
    std::string inviteClient(Client client, std::string clien_to_add, std::vector<Client> &Clients);
    std::string kickClient(Client client, std::string clien_to_kick);
    void sendMessage(std::string message, Client &client);
    std::string sendPrivateMessage(std::string message, Client &client, std::string to);
    std::string showTopic(Client client);
    std::string setTopic(Client client, std::string topic);
    std::string getChannelName();
    bool isPrivate();
    std::string getPassword();
    std::vector<Client> getClients();
};

#endif
