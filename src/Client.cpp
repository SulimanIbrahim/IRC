#include "../include/Client.hpp"

Client::Client(int _clientSockets):password_entered(false), _fd(_clientSockets), _username(""), _nickname(""), at_channel("General") {
    std::cout << "Client constructor" << std::endl;
    std::cout << "Client socket: " << _clientSockets << std::endl;
    _realname = "";
    _hostname = "";
}

Client::~Client()
{
    std::cout << "Client destructor has been called" << std::endl;
}

Client::Client(const Client &client)
{
    std::cout << "Client copy constructor" << std::endl;
    _fd = client._fd;
    _username = client._username;
    _nickname = client._nickname;
    at_channel = client.at_channel;
    password_entered = client.password_entered;
    _realname = client._realname;
    _hostname = client._hostname;
    _bot = client._bot; // Copy bot data
}

// Bot related methods for Client
void Client::addActivity(const std::string& activity) {
    _bot.addActivity(activity);
}

std::string Client::getBotResponse() {
    return _bot.getResponse(_username);
}

bool Client::isAuthentificated()
{
    if (_username != "" && _nickname != "" && password_entered)
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

void Client::setChannel(std::string channel)
{
    at_channel = channel;
}

std::string Client::getChannel()
{
    return at_channel;
};

int Client::get_fd()
{
    return _fd;
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