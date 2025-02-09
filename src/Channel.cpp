#include "../include/Channel.hpp"

Channel::Channel(Client client, std::string ChannelName): _ChannelName(ChannelName)
{
    Clients.push_back(client);
};

Channel::~Channel()
{
};

std::string Channel::addClient(Client client)
{
    std::vector<Client>::iterator it = Clients.begin();
    for (; it != Clients.end(); ++it)
    {
        if (it->get_nick() == client.get_nick())
            break;
    }
    if (it == Clients.end()) {
        Clients.push_back(client);
        return client.get_username() + " has been added to channel " + _ChannelName + "\n";
    }
    else
        return "Client " + client.get_username() + " already in channel " + _ChannelName + "\n";
};

void Channel::sendMessage(std::string message, Client &client)
{
    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it)
    {
        if (it->get_username() != client.get_username())
            send(it->get_pfd().fd, message.c_str(), message.size(), 0);
    }
};

std::string Channel::kickClient(Client client)
{
    std::vector<Client>::iterator it = Clients.begin();
    for (; it != Clients.end(); ++it)
    {
        if (it->get_nick() == client.get_nick())
            break;
    }
    if (it != Clients.end()) {
        Clients.erase(it);
        return client.get_username() + " has been kicked from channel " + _ChannelName + "\n";
    }
    else
        return "Client " + client.get_username() + " not found in channel " + _ChannelName + "\n";
};

std::string Channel::getChannelName()
{
    return _ChannelName;
};

std::vector<Client> Channel::getClients()
{
    return Clients;
};