#include "../include/Client.hpp"

Client::Client(int _clientSockets):password_entered(false), _username(""), _nickname(""), at_channel("") {
    _pfd.fd = _clientSockets;
    _pfd.events = POLLIN;
    _pfd.revents = 0;
}
Client::~Client()
{

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

pollfd Client::get_pfd()
{
    return _pfd;
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