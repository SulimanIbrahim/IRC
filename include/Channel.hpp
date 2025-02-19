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
#include <map>



struct DCCRequest {
    std::string filename;
    std::string ip;
    int port;
    int filesize;
    bool accepted;
};

class Channel {
private:
    const std::string _ChannelName;
    std::vector<Client> _Clients;
    std::map<std::string, DCCRequest> _dcc_requests;

public:
    Channel(Client client, std::string ChannelName);
    ~Channel();

    std::string inviteClient(Client client, std::string clien_to_add, std::vector<Client> &Clients);
    std::string kickClient(Client client, std::string clien_to_kick);
    void sendMessage(std::string message, Client &client);
    std::string sendPrivateMessage(std::string message, Client &client, std::string to);
    std::string sendDCCRequest(std::string filename, Client &client, std::string to);
    std::string acceptDCCRequest(std::string client, std::string filename);
    std::string setupDCC_Connection(std::string client, std::string filename);
    std::string rejectDCCRequest(std::string client, std::string filename);
    std::string getChannelName();
    std::vector<Client> getClients();
};

#endif
