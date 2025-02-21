#include "../include/Channel.hpp"

Channel::Channel(Client client, std::string ChannelName): _ChannelName(ChannelName)
{
    _Clients.push_back(client);
    _operators.push_back(client);
    _Topic = "";
    _Password = "";
    i = false;
    t = false;
    k = false;
};

Channel::Channel(Client client, std::string ChannelName, std::string password): _ChannelName(ChannelName), _Password(password)
{
    _Clients.push_back(client);
    _operators.push_back(client);
    _Topic = "";
    i = false;
    t = false;
    k = true;
};

Channel::~Channel()
{
};


std::string Channel::inviteClient(Client client, std::string client_to_add, std::vector<Client> &Clients)
{
    if (isInviteOnly())
    {
        for (std::vector<Client>::iterator op = _operators.begin(); op != _operators.end(); ++op)
        {
            if (op->get_username() == client.get_username())
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
            }
        }
        return "The channel is invite only\n";
    }
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
    std::vector<Client>::iterator it = _operators.begin();
    for (; it != _operators.end(); ++it)
    {
        if (it->get_username() == client.get_username())
            break;
    }
    if (it == _operators.end())
        return "Sorry, But you are not an operator in channel " + _ChannelName + "\n";
    it = _Clients.begin();
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
    if (isTopicRisterction())
    {
        for (std::vector<Client>::iterator it = _operators.begin(); it != _operators.end(); ++it)
        {
            if (it->get_username() == client.get_username())
                return "Topic: " + _Topic + "\n";
        }
        return "The topic is restricted and only operators can view it\n";
    }
    return "Topic: " + _Topic + "\n";
};

void Channel::setTopic(std::string topic)
{
    _Topic = topic;
};


std::string Channel::getChannelName()
{
    return _ChannelName;
};

std::vector<Client> Channel::getClients()
{
    return _Clients;
};

std::string Channel::setInviteOnly_status(std::vector<std::string> tokens)
{
    if (tokens.size() > 2)
        return "Invalid mode\n";
    if (tokens[1][0] == '+')
    {
        i = true;
        return "Invite only mode enabled\n";
    }
    if (tokens[1][0] == '-')
    {
        i = false;
        return "Invite only mode disabled\n";
    }
    return "Invalid mode\n";
};

std::string Channel::setTopicRisterction_status(std::vector<std::string> tokens)
{
    if (tokens.size() == 3)
        setTopic(tokens[2]);
    else if (_Topic == "")
        return "Topic required\n";
    if (tokens[1][0] == '+')
    {
        t = true;
        return "Topic restriction mode enabled\n";
    }
    if (tokens[1][0] == '-')
    {
        t = false;
        return "Topic restriction mode disabled\n";
    }
    return "Invalid mode\n";
};

std::string Channel::setPrivate_status(std::vector<std::string> tokens)
{
    if (tokens.size() == 3)
        setPassword(tokens[2]);
    if (getPassword() == "")
        return "Password required\n";
    if (tokens[1][0] == '+')
    {
        k = true;
        return "Password is required\n";
    }
    if (tokens[1][0] == '-')
    {
        k = false;
        return "Password is not required\n";
    }
    return "Invalid mode\n";
};

std::string Channel::setOperator_status(std::vector<std::string> tokens)
{
        if (tokens.size() < 3)
            return "requires a username\n";
        if (tokens[1][0] == '+')
        {
            for (std::vector<Client>::iterator it = _operators.begin(); it != _operators.end(); ++it)
            {
                if (it->get_username() == tokens[2])
                    return "Operator already in channel\n";
            }
            for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
            {
                if (it->get_username() == tokens[2])
                {
                    _operators.push_back(*it);
                    return "Operator added\n";
                }
            }
            return "Client " + tokens[2] + " not found\n";
        }
        if (tokens[1][0] == '-')
        {
            for (std::vector<Client>::iterator it = _operators.begin(); it != _operators.end(); ++it)
            {
                if (it->get_username() == tokens[2])
                {
                    _operators.erase(it);
                    return "Operator removed\n";
                }
            }
            return "Operator " + tokens[2] + " not found\n";
        }
    return "Invalid mode\n";
};

void Channel::setPassword(std::string password)
{
    _Password = password;
};

bool Channel::isInviteOnly()
{
    return i;
};

bool Channel::isTopicRisterction()
{
    return t;
};

bool Channel::isPrivate()
{
    return k;
};

std::string Channel::getPassword()
{
    return _Password;
};

std::string Channel::mode(Client client, std::vector<std::string> tokens) {
    if (tokens.size() > 3)
        return "Too many arguments\n";
    std::vector<Client>::iterator it = _operators.begin();
    for (; it != _operators.end(); ++it)
    {
        if (it->get_username() == client.get_username())
            break;
    }
    if (it == _operators.end())
        return "Sorry, But you are not an operator in channel " + _ChannelName + "\n";
    if (tokens.size() == 1)
        return status();
    if (tokens[1].length() > 2)
        return "Invalid mode\n" + status();
    if (tokens[1][1] == 't')
        return setTopicRisterction_status(tokens);
    if (tokens[1][1] == 'i')
        return setInviteOnly_status(tokens);
    if (tokens[1][1] == 'k')
        return setPrivate_status(tokens);
    if (tokens[1][1] == 'o')
        return setOperator_status(tokens);
    return "Invalid mode\n";
}

std::string Channel::status()
{
    std::string status = BLUE;
    status += "Invite only mode: ";
    status += i ? "enabled" : "disabled";
    status += "\n";
    status += "Topic restriction mode: ";
    status += t ? "enabled" : "disabled";
    status += "\n";
    status += "Password: ";
    status += k ? "required" : "not required";
    status += RESET;
    return status;
};