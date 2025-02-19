#include "../include/Channel.hpp"

Channel::Channel(Client client, std::string ChannelName): _ChannelName(ChannelName)
{
    _Clients.push_back(client);
};

Channel::~Channel()
{
};

std::string Channel::inviteClient(Client client, std::string client_to_add, std::vector<Client> &Clients)
{
    std::vector<Client>::iterator it = _Clients.begin();
    for (; it != _Clients.end(); ++it)
    {
        if (it->get_username() == client_to_add)
            break;
    }
    if (it == _Clients.end()) {
        for (it = Clients.begin(); it != Clients.end(); ++it)
        {
            if (it->get_username() == client_to_add)
            {
                _Clients.push_back(*it);
                return client.get_username() + " added " + client_to_add + " to channel " + _ChannelName + "\n";
            }
        }
        return "Client " + client_to_add + " not found\n";
    }
    return "Client " + client.get_username() + " already in channel " + _ChannelName + "\n";
};

void Channel::sendMessage(std::string message, Client &client)
{
    for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
    {
        if (it->get_username() != client.get_username())
            send(it->get_fd(), message.c_str(), message.size(), 0);
    }
};

std::string Channel::sendPrivateMessage(std::string message, Client &client, std::string to)
{
    std::string message_to_send = client.get_username() + " whispers: " + message + "\n";
    for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
    {
        if (it->get_username() == to)
        {
            send(it->get_fd(), message_to_send.c_str(), message.size(), 0);
            return "";
        }
    }
    return "Client " + to + " not found in channel " + _ChannelName + "\n";
};


std::string Channel::kickClient(Client client, std::string client_to_kick)
{
    std::vector<Client>::iterator it = _Clients.begin();
    for (; it != _Clients.end(); ++it)
    {
        if (it->get_username() == client_to_kick)
        {
            _Clients.erase(it);
            return client.get_username() + " kicked " + client_to_kick + " from channel " + _ChannelName + "\n";
        }
    }
    return "Client " + client_to_kick + " not found in channel " + _ChannelName + "\n";
};

std::string Channel::sendDCCRequest(std::string filename, Client &client, std::string to)
{
    std::string message_to_send = client.get_username() + " wants to send you a file: " + filename + "\n";
    for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
    {
        if (it->get_username() == to)
        {
            DCCRequest request = {filename, client.get_ip(), 0, 20, false};
            _dcc_requests[to] = request;
            if (send(it->get_fd(), message_to_send.c_str(), message_to_send.size(), 0) < 0)
                return "Failed to send message\n";
        }
    }
    return "Client " + to + " not found in channel " + _ChannelName + "\n";
};

std::string Channel::acceptDCCRequest(std::string client, std::string filename)
{
    std::map<std::string, DCCRequest>::iterator it = _dcc_requests.find(client);
    if (it != _dcc_requests.end())
    {
        if (it->second.filename == filename)
        {
            it->second.accepted = true;
            return setupDCC_Connection(client, filename);
        }
    }
    return "NO REQUEST FOUND\n";
};

std::string Channel::setupDCC_Connection(std::string client, std::string filename)
{
    std::map<std::string, DCCRequest>::iterator it = _dcc_requests.find(client);
    if (it != _dcc_requests.end())
    {
        if (it->second.filename == filename)
        {
            if (it->second.accepted)
            {
                sockaddr_in client_addr;
                client_addr.sin_family = AF_INET;
                client_addr.sin_port = htons(it->second.port);
                inet_pton(AF_INET, it->second.ip.c_str(), &client_addr.sin_addr);
                int dcc_fd = socket(AF_INET, SOCK_STREAM, 0);
                if (dcc_fd < 0)
                    return "Failed to create socket\n";
                if (connect(dcc_fd, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0)
                    return "Failed to connect to client\n";
                if (send(dcc_fd, "DCC ACCEPTED\n", 13, 0) < 0)
                    return "Failed to send message\n";
                return "DCC connection established\n";
            }
            return "DCC request not accepted\n";
        }
    }
    return "NO REQUEST FOUND\n";
};


std::string Channel::rejectDCCRequest(std::string client, std::string filename)
{
    std::map<std::string, DCCRequest>::iterator it = _dcc_requests.find(client);
    if (it != _dcc_requests.end())
    {
        if (it->second.filename == filename)
        {
            it->second.accepted = false;
            return "DCC request rejected\n";
        }
    }
    return "NO REQUEST FOUND\n";
};

std::string Channel::getChannelName()
{
    return _ChannelName;
};

std::vector<Client> Channel::getClients()
{
    return _Clients;
};