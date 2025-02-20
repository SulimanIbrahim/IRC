#include "../include/Channel.hpp"

Channel::Channel(Client client, std::string ChannelName): _ChannelName(ChannelName)
{
    _Clients.push_back(client);
    _operators.push_back(client);
    _Topic = "";
    _Password = "";
    _isPrivate = false;
};

Channel::Channel(Client client, std::string ChannelName, std::string password): _ChannelName(ChannelName), _Password(password)
{
    _Clients.push_back(client);
    _operators.push_back(client);
    _Topic = "";
    _isPrivate = true;
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
            return "Client " + client.get_username() + " already in channel " + _ChannelName + "\n";
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

std::string Channel::listClients()
{
    std::string clients = "Clients in channel " + _ChannelName + ":\n";
    for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
    {
        clients += it->get_username() + "\n";
    }
    return clients;
};

void Channel::sendMessage(std::string message, Client &client)
{
    std::string message_to_send = client.get_username() + ": " + message + "\n";
    for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
    {
        if (it->get_username() != client.get_username())
            send(it->get_fd(), message_to_send.c_str(), message_to_send.size(), 0);
    }
};

std::string Channel::sendPrivateMessage(std::string message, Client &client, std::string to)
{
    std::string message_to_send = client.get_username() + " whispers: " + message + "\n";
    std::cout << message_to_send << std::endl;
    for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
    {
        if (it->get_username() == to)
        {
            send(it->get_fd(), message_to_send.c_str(), message_to_send.size(), 0);
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

std::string Channel::showTopic(Client client)
{
    for (std::vector<Client>::iterator it = _operators.begin(); it != _operators.end(); ++it)
    {
        if (it->get_username() == client.get_username())
            return "Topic: " + _Topic + "\n";
    }
    return "You are not an operator in channel " + _ChannelName + "\n";
};

std::string Channel::setTopic(Client client, std::string topic)
{
    for (std::vector<Client>::iterator it = _operators.begin(); it != _operators.end(); ++it)
    {
        if (it->get_username() == client.get_username()){
            _Topic = topic;
            return "Topic set to " + topic + "\n";
        }
    }
    return "You are not an operator in channel " + _ChannelName + "\n";
};


std::string Channel::getChannelName()
{
    return _ChannelName;
};

std::vector<Client> Channel::getClients()
{
    return _Clients;
};

bool Channel::isPrivate()
{
    return _isPrivate;
};

std::string Channel::getPassword()
{
    return _Password;
};