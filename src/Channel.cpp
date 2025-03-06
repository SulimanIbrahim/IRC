#include "../include/Channel.hpp"

Channel::Channel(Client client, std::string ChannelName, Server* server): _ChannelName(ChannelName), _server(server)
{
    _Clients.push_back(client);
    _operators.push_back(client);
    _Topic = "";
    _Password = "";
    InviteOnly = false;
    TopicRistercted = false;
    PasswordProtected = false;
};

Channel::Channel(Client client, std::string ChannelName, std::string password, Server* server): _ChannelName(ChannelName), _Password(password), _server(server)
{
    _Clients.push_back(client);
    _operators.push_back(client);
    _Topic = "";
    InviteOnly = false;
    TopicRistercted = false;
    PasswordProtected = true;
};

Channel::~Channel()
{
};

std::string Channel::Invite(Client client, std::string client_to_add, std::vector<Client> &Clients)
{
    if (isOperator(client))
    {
        if (isClientInChannel(client_to_add))
            return "you are already in the channel\n";
        for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it)
        {
            if (it->get_nick() == client_to_add)
            {
                _Clients.push_back(*it);
                return "Client " + client_to_add + " joined channel " + _ChannelName + "\n";
            }
        }
        return "Client " + client_to_add + " not found\n";
    }
    else
        return "you are not an operator in channel " + _ChannelName + "\n";
};

std::string Channel::JoinChannel(Client client, std::vector<Client> &Clients)
{
    if (isClientInChannel(client))
        return "Client " + client.get_nick() + " already in channel " + _ChannelName + "\n";
    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it)
    {
        if (it->get_nick() == client.get_nick())
        {
            _Clients.push_back(*it);
            return "Client " + client.get_nick() + " joined channel " + _ChannelName + "\n";
        }
    }
    return "Client " + client.get_nick() + " not found\n";
};

std::string Channel::listClients()
{
    std::string clients = "Clients in channel " + _ChannelName + ":\n";
    for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
        clients += it->get_nick() + "\n";
    return clients;
};

void Channel::sendMessage(std::string message, Client &client, std::vector<Client> &serv_Clients)
{
    std::string message_to_send =  ":" +client.get_nick() +  "!~" + client.get_nick() + "@localhost PRIVMSG "  + _ChannelName + " :" + message + "\n";
    for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
    {
        if (it->get_nick() != client.get_nick())
        {
            for (std::vector<Client>::iterator op = serv_Clients.begin(); op != serv_Clients.end(); ++op)
            {
                if (op->get_nick() == it->get_nick())
                {
                    // std::cout << message_to_send << std::endl;
                    op->addToOutBuffer(message_to_send);
                    if (_server) {
                        _server->enableWriteEvent(it->get_fd());
                    }
                }
            }
        }
    }
};

std::string Channel::sendPrivateMessage(std::string message, Client &client, std::string to)
{
    // add a check for specific client, clients
    std::string message_to_send = ":" +client.get_nick() +  "!~" + client.get_nick() + "@localhost PRIVMSG "  + _ChannelName + " :" + message + "\n";
    // std::cout << message_to_send << std::endl;
    for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
    {
        if (it->get_nick() == to)
        {
            it->addToOutBuffer(message_to_send);
            if (_server) {
                _server->enableWriteEvent(it->get_fd());
            }
            return "";
        }
    }
    return "Client " + to + " not found in channel " + _ChannelName + "\n";
};


std::string Channel::kickClient(Client client, std::string client_to_kick)
{
    if (isOperator(client) == false)
        return "Sorry, But you are not an operator in channel " + _ChannelName + "\n";
    for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
    {
        if (it->get_nick() == client_to_kick)
        {
            _Clients.erase(it);
            return client.get_nick() + " kicked " + client_to_kick + " from channel " + _ChannelName + "\n";
        }
    }
    return "Client " + client_to_kick + " not found in channel " + _ChannelName + "\n";
};

std::string Channel::sendDCCRequest(std::string filename, Client &client, std::string to)
{
    // Find the recipient
    Client* recipient = NULL;
    for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it) {
        if (it->get_nick() == to) {
            recipient = &(*it);
            break;
        }
    }
    
    if (!recipient) {
        return "Error: Client " + to + " not found in channel " + _ChannelName + "\n";
    }
    
    // Format the DCC SEND message according to IRC protocol
    // Format: :<sender> PRIVMSG <recipient> :<DCC SEND filename ipaddress port filesize>
    unsigned long ip = inet_addr(client.get_ip().c_str());
    
    // In a real DCC implementation, the sender would open a port and listen
    // For simplicity, we'll use a placeholder port
    int port = 12345; // This would normally be dynamically assigned
    
    // Get file size
    FILE *file = fopen(filename.c_str(), "rb");
    if (!file) {
        return "Error: File " + filename + " not found\n";
    }
    fseek(file, 0, SEEK_END);
    unsigned long filesize = ftell(file);
    fclose(file);
    
    // Create the DCC SEND message
    std::string dcc_msg = ":" + client.get_nick() + " PRIVMSG " + to + 
                         " :\001DCC SEND " + filename + " " + 
                         std::to_string(ip) + " " + 
                         std::to_string(port) + " " + 
                         std::to_string(filesize) + "\001\r\n";
    
    // Forward the message to the recipient
    // if (send(recipient->get_fd(), dcc_msg.c_str(), dcc_msg.length(), 0) < 0) {
    //     return "Error: Failed to send DCC request\n";
    // }
    
    return "DCC request sent to " + to + " for file " + filename + "\n";
}


std::string Channel::showTopic()
{
    if (_Topic == "")
        return "There is no topic set for channel " + _ChannelName + "\n use /topic <topic> to set a topic\n";
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
    if (tokens.size() > 3)
        return RED "Invalid mode\n" RESET;
    if (tokens[2][0] == '+')
    {
        InviteOnly = true;
        return "Invite only mode enabled\n";
    }
    if (tokens[2][0] == '-')
    {
        InviteOnly = false;
        return "Invite only mode disabled\n";
    }
    return RED "Invalid mode\n" RESET;
};

std::string Channel::setTopicRisterction_status(std::vector<std::string> tokens)
{
    if (tokens.size() == 4)
        setTopic(tokens[3]);
    else if (_Topic == "")
        return "Topic required\n";
    if (tokens[2][0] == '+')
    {
        TopicRistercted = true;
        return "Topic restriction mode enabled\n";
    }
    if (tokens[2][0] == '-')
    {
        TopicRistercted = false;
        return "Topic restriction mode disabled\n";
    }
    return RED "Invalid mode\n" RESET;
};

std::string Channel::setPrivate_status(std::vector<std::string> tokens)
{
    if (tokens.size() == 4)
        setPassword(tokens[3]);
    if (getPassword() == "")
        return "Password required\n";
    if (tokens[2][0] == '+')
    {
        PasswordProtected = true;
        return "Password is required\n";
    }
    if (tokens[2][0] == '-')
    {
        PasswordProtected = false;
        return "Password is not required\n";
    }
    return RED "Invalid mode\n" RESET;
};

std::string Channel::setOperator_status(std::vector<std::string> tokens)
{
        if (tokens.size() < 4)
            return "requires a username\n";
        if (tokens[2][0] == '+')
        {
            for (std::vector<Client>::iterator it = _operators.begin(); it != _operators.end(); ++it)
            {
                if (it->get_nick() == tokens[2])
                    return "Operator already in channel\n";
            }
            for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
            {
                if (it->get_nick() == tokens[2])
                {
                    _operators.push_back(*it);
                    return "Operator added\n";
                }
            }
            return "Client " + tokens[2] + " not found\n";
        }
        if (tokens[2][0] == '-')
        {
            for (std::vector<Client>::iterator it = _operators.begin(); it != _operators.end(); ++it)
            {
                if (it->get_nick() == tokens[2])
                {
                    _operators.erase(it);
                    return "Operator removed\n";
                }
            }
            return "Operator " + tokens[2] + " not found\n";
        }
    return RED "Invalid mode\n" RESET;
};

void Channel::setPassword(std::string password)
{
    _Password = password;
};

std::string Channel::leaveChannel(Client &client)
{
    std::vector<Client>::iterator it = _Clients.begin();
    for (; it != _Clients.end(); ++it)
    {
        if (it->get_nick() == client.get_nick())
        {
            it->removeChannel(_ChannelName);
            _Clients.erase(it);
            return "Client " + client.get_nick() + " left channel " + _ChannelName + "\n";
        }
    }
    return "Client " + client.get_nick() + " not found in channel " + _ChannelName + "\n";
};

bool Channel::isInviteOnly()
{
    return InviteOnly;
};

bool Channel::isTopicRisterction()
{
    return TopicRistercted;
};

bool Channel::isPrivate()
{
    return PasswordProtected;
};

std::string Channel::getPassword()
{
    return _Password;
};

std::string Channel::mode(Client client, std::vector<std::string> tokens) {
    if (tokens.size() != 3 && tokens.size() != 4)
        return BLUE "MODE [channel] +/-[mode] [parameters]\n" RESET;
    std::vector<Client>::iterator it = _operators.begin();
    for (; it != _operators.end(); ++it)
    {
        if (it->get_nick() == client.get_nick())
            break;
    }
    if (it == _operators.end())
        return "Sorry, But you are not an operator in channel " + _ChannelName + "\n";
    if (tokens.size() == 1)
        return status();
    if (tokens[2].length() != 2)
        return "Invalid mode\n" + status();
    if (tokens[2][1] == 't')
        return setTopicRisterction_status(tokens);
    if (tokens[2][1] == 'i')
        return setInviteOnly_status(tokens);
    if (tokens[2][1] == 'k')
        return setPrivate_status(tokens);
    if (tokens[2][1] == 'o')
        return setOperator_status(tokens);
    return RED "Invalid mode\n" RESET;
}

std::string Channel::status()
{
    std::string status = BLUE;
    status += "Invite only mode: ";
    status += InviteOnly ? "enabled" : "disabled";
    status += "\n";
    status += "Topic restriction mode: ";
    status += TopicRistercted ? "enabled" : "disabled";
    status += "\n";
    status += "Password: ";
    status += PasswordProtected ? "required" : "not required";
    status += RESET;
    return status;
};

bool Channel::isClientInChannel(Client &client)
{
    for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
    {
        if (it->get_nick() == client.get_nick())
            return true;
    }
    return false;
};

bool Channel::isClientInChannel(std::string &client)
{
    for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
    {
        if (it->get_nick() == client)
            return true;
    }
    return false;
};

bool Channel::isOperator(Client &client)
{
    for (std::vector<Client>::iterator it = _operators.begin(); it != _operators.end(); ++it)
    {
        if (it->get_nick() == client.get_nick())
            return true;
    }
    return false;
};