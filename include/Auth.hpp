#ifndef AUTH_HPP
#define AUTH_HPP

// #include "Server.hpp"
#include "Client.hpp"
#include <iostream>
#include <vector>
#include <string>

class Auth {
    public :
    virtual ~Auth();
    virtual std::string runAuthCommands(Client &client, const std::vector<std::string>& tokens, std::string password) = 0;
};

class Pass : public Auth {
    public :
    std::string runAuthCommands(Client &client, const std::vector<std::string>& tokens, std::string password);
};

class Nick : public Auth {
    public :
    std::string runAuthCommands(Client &client, const std::vector<std::string>& tokens, std::string ignore);
};

class User : public Auth {
    public :
    std::string runAuthCommands(Client &client, const std::vector<std::string>& tokens, std::string ignore);
};


#endif

