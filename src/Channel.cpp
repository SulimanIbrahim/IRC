#include "../include/Channel.hpp"

Channel::Channel(Client client, std::string ChannelName): _ChannelName(ChannelName)
{
    Clients.push_back(client);
};

Channel::~Channel()
{
};

void Channel::addClient(Client client)
{
    std::vector<Client>::iterator it = Clients.begin();
    for (; it != Clients.end(); ++it)
    {
        if (it->get_nick() == client.get_nick())
            break;
    }
    if (it == Clients.end())
        Clients.push_back(client);
    else
        std::cout << "Client "<< client.get_username() << " already in channel " << _ChannelName << std::endl;
};

void Channel::kickClient(Client client)
{
    std::vector<Client>::iterator it = Clients.begin();
    for (; it != Clients.end(); ++it)
    {
        if (it->get_nick() == client.get_nick())
            break;
    }
    if (it != Clients.end())
        Clients.erase(it);
    else
        std::cout << "Client "<< client.get_username() << " not in channel " << _ChannelName << std::endl;
};

std::string Channel::getChannelName()
{
    return _ChannelName;
};

std::vector<Client> Channel::getClients()
{
    return Clients;
};