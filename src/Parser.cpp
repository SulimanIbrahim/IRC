#include "../include/Parser.hpp"

Parser::Parser(int ac, char **av) {
    if (ac != 3) {
        throw std::runtime_error("Error: Invalid number of arguments. Usage: ./server <port> <password>");
    }
    
    checkNonNumeric(av[1]);
    checkNegativeNumber(av[1]);
    checkValidPort(std::atoi(av[1]));

    _port = std::atoi(av[1]);
    _password = av[2];

    std::cout << "Parser initialized: Port = " << _port << ", Password = " << _password << std::endl;
}

Parser::~Parser() {}

void Parser::checkNonNumeric(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            throw std::runtime_error("Error: Port must be a number.");
        }
    }
}

void Parser::checkNegativeNumber(const char *str) {
    if (str[0] == '-') {
        throw std::runtime_error("Error: Port must be a positive number.");
    }
}

void Parser::checkValidPort(int port) {
    if (port < 1024 || port > 49151) {
        throw std::runtime_error("Error: Port must be between 1024 and 49151.");
    }
}

std::vector<std::string> Parser::split(std::string str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    for (unsigned int i = 0; i < str.size(); i++) {
        if (str[i] == delimiter) {
            tokens.push_back(token);
            token.clear();
        } else {
            token += str[i];
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

int Parser::getPort() const {
    return _port;
}

std::string Parser::getPassword() const {
    return _password;
}
