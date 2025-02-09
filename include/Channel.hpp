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
    std::vector<Client> Clients;

public:
    Channel(Client client, std::string ChannelName);
    ~Channel();

    std::string addClient(Client client);
    std::string kickClient(Client client);
    void sendMessage(std::string message, Client &client);
    std::string getChannelName();
    std::vector<Client> getClients();
};

#endif
