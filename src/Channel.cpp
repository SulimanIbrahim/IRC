#include "../include/Channel.hpp"

Channel::Channel(Client client, std::string ChannelName): _ChannelName(ChannelName)
{
    _Clients.push_back(client);
};

Channel::~Channel()
{
};

std::string Channel::addClient(Client client, std::string client_to_add, std::vector<Client> &Clients)
{
    std::vector<Client>::iterator it = _Clients.begin();
    for (; it != _Clients.end(); ++it)
    {
        if (it->get_nick() == client.get_nick())
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
    }
    return "Client " + client.get_username() + " already in channel " + _ChannelName + "\n";
};

void Channel::sendMessage(std::string message, Client &client)
{
    for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
    {
        if (it->get_username() != client.get_username())
            send(it->get_pfd().fd, message.c_str(), message.size(), 0);
    }
};

std::string Channel::sendPrivateMessage(std::string message, Client &client, std::string to)
{
    std::string message_to_send = client.get_username() + " whispers: " + message + "\n";
    for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
    {
        if (it->get_username() == to)
        {
            send(it->get_pfd().fd, message_to_send.c_str(), message.size(), 0);
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
            break;
    }
    if (it != _Clients.end()) {
        _Clients.erase(it);
        return client.get_username() + " kicked " + client_to_kick + " from channel " + _ChannelName + "\n";
    }
    else
        return "Client " + client_to_kick + " not found in channel " + _ChannelName + "\n";
};

std::string Channel::getChannelName()
{
    return _ChannelName;
};

std::vector<Client> Channel::getClients()
{
    return _Clients;
};