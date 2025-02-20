#include "../include/Client.hpp"

Client::Client(int _clientSockets):password_entered(false), _username(""), _nickname(""), at_channel("") {
    std::cout << "Client constructor" << std::endl;
    std::cout << "Client socket: " << _clientSockets << std::endl;
    _fd = _clientSockets;
    _username = "Client";
    _nickname = "Client";
    at_channel = "General";
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
}

void Client::set_nick(std::string nickname)
{
    _nickname = nickname;
};
void Client::set_username(std::string username)
{
    _username = username;
};

void Client::setChannel(std::string channel)
{
    at_channel = channel;
};

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
};