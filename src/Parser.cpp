#include "../include/Parser.hpp"

Parser::Parser(int ac, char **av) {
    if (ac != 3) {
        std::cerr << "Usage: ./ircserv [port] [password]" << std::endl;
        exit(EXIT_FAILURE);
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
            std::cerr << "Error: Port must be numeric." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

void Parser::checkNegativeNumber(const char *str) {
    if (str[0] == '-') {
        std::cerr << "Error: Port must be a positive number." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Parser::checkValidPort(int port) {
    if (port < 1024 || port > 49151) {
        std::cerr << "Error: Port must be between 1024 and 49151." << std::endl;
        exit(EXIT_FAILURE);
    }
}

int Parser::getPort() const {
    return _port;
}

std::string Parser::getPassword() const {
    return _password;
}
