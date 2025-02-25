#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <cstdlib>
#include <string>
#include <cctype>
#include <vector>
#define RED "\033[31m"
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"

class Parser {
private:
    int _port;
    std::string _password;

    void checkNonNumeric(const char *str);
    void checkNegativeNumber(const char *str);
    void checkValidPort(int port);

public:
    Parser(int ac, char **av);
    ~Parser();

    static std::vector<std::string> split(std::string str, char delimiter);
    static std::string take_first_line(std::string str);
    static std::string join_message(std::vector<std::string> tokens);
    int getPort() const;
    std::string getPassword() const;
};

#endif
