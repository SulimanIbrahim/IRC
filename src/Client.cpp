#include "../include/Client.hpp"

Client::Client(int _clientSockets, std::string ip):password_entered(false), _username(""), _nickname("") {
    _fd = _clientSockets;
    _ip = ip;
    _username = "";
    _nickname = "";
    _realname = "";
    _hostname = "";
}

Client::~Client()
{}

Client::Client(const Client &client)
{
    _fd = client._fd;
    _username = client._username;
    _nickname = client._nickname;
    for (std::vector<std::string>::const_iterator it = client.channels.begin(); it != client.channels.end(); ++it)
    {
        channels.push_back(*it);
    }
    password_entered = client.password_entered;
    registered = client.registered;
    _realname = client._realname;
    _hostname = client._hostname;
    _bot = client._bot; // Copy bot data
}

// Bot related methods for Client
void Client::addActivity(const std::string& activity) {
    _bot.addActivity(activity);
}

std::string Client::getBotResponse(std::string channel) {
    return _bot.getResponse(channel, _nickname, _username, _hostname);
}

bool Client::isAuthentificated()
{
    if (password_entered and registered)
        return true;
    return false;
};

void Client::set_nick(std::string nickname)
{
    _nickname = nickname;
    
    if (!nickname.empty()) {
        addActivity("Changed nickname to " + nickname);
    }
}
void Client::set_username(std::string username)
{
    _username = username;
    
    if (!username.empty()) {
        addActivity("Set username to " + username);
    }
}

void Client::addChannel(std::string channel)
{
    if (!is_inChannel(channel))
    {
        channels.push_back(channel);
    }
}

void Client::removeChannel(std::string channel)
{
    if (is_inChannel(channel))
    {
        channels.erase(std::remove(channels.begin(), channels.end(), channel), channels.end());
    }
}



int Client::get_fd()
{
    return _fd;
};

std::string Client::get_ip()
{
    return _ip;
};
std::string Client::get_nick(){
    return _nickname;
};
std::string Client::get_username(){
    return _username;
};

bool Client::isPasswordEntered()
{
    return password_entered;
};

void Client::setPasswordEntered(bool value)
{
    password_entered = value;
    
    if (value) {
        addActivity("Password authenticated");
    }
}

void Client::set_realname(std::string realname)
{
    _realname = realname;
};

std::string Client::get_realname()
{
    return _realname;
};

void Client::set_hostname(std::string hostname)
{
    _hostname = hostname;
};


std::string Client::get_hostname()
{
    return _hostname;
};

void Client::addToOutBuffer(const std::string& msg) {
    _outBuffer += msg;
}

std::string& Client::getOutBuffer() {
    return _outBuffer;
}

bool Client::is_inChannel(std::string channel)
{
    for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        if (*it == channel)
            return true;
    }
    return false;
}

bool Client::isRegistered()
{
    return registered;
}

void Client::setRegistered(bool value)
{
    registered = value;
    
    if (value) {
        addActivity("Registered");
    }
}