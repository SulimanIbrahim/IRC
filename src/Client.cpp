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
    // close(_fd);
}

void Client::set_nick(std::string nickname)
{
    // adding a parsing function to check if the nickname is valid
    _nickname = nickname;
};
void Client::set_username(std::string username)
{
    // adding a parsing function to check if the username is valid
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