#include "../include/Auth.hpp"
#include "../include/Server.hpp"


Auth::~Auth() {
    
}

static std::string sendWelcomeMessage(Client &client) {
    std::string welcome;
    std::string userhost = client.get_nick() + "!" + client.get_username() + "@" + client.get_hostname();
    welcome = ":" + std::string(SERVER_NAME) + " 001 " + client.get_nick() + " :Welcome to the Internet Relay Network " + userhost + "\r\n";
    welcome += ":" + std::string(SERVER_NAME) + " 002 " + client.get_nick() + " :Your host is " + std::string(SERVER_NAME) + ", running version 1.0\r\n";
    welcome += ":" + std::string(SERVER_NAME) + " 003 " + client.get_nick() + " :This server was created today\r\n";
    welcome += ":" + std::string(SERVER_NAME) + " 004 " + client.get_nick() + " " + std::string(SERVER_NAME) + " 1.0 o o\r\n";
    return welcome;
}

std::string Pass::runAuthCommands(Client &client, const std::vector<std::string>& tokens, std::string password, std::vector<Client> &ignore_Clients) {
    (void)ignore_Clients;

    if (tokens.size() < 2) {
        return ":" + std::string(SERVER_NAME) + " 461 * PASS :Not enough parameters\r\n";
    }
    std::cout << "Password entered: " << tokens[1] << std::endl;
    std::cout << "Password expected: " << password << std::endl;
    if (password == tokens[1]) {
        client.setPasswordEntered(true);
        return "332 * :You may now register\r\n";
    }

    return ":" + std::string(SERVER_NAME) + " 464 * :Password incorrect\r\n";
}

std::string Nick::runAuthCommands(Client &client, const std::vector<std::string>& tokens, std::string password, std::vector<Client> &Clients) {
    (void)password;
    if (tokens.size() < 2) {
        return ERR_NOTENOUGHPARAM(tokens[1]);
    }

    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it) {
        if (it->get_nick() == tokens[1]) {
            return ERR_NICKINUSE(tokens[1]);
        }
    }

    client.set_nick(tokens[1]);
    if (client.get_nick() != "" and client.get_username() != "" and client.isRegistered() == false) {
        client.setRegistered(true);
        return sendWelcomeMessage(client);
    }
    return "";
}

std::string User::runAuthCommands(Client &client, const std::vector<std::string>& tokens, std::string password, std::vector<Client> &Clients) {
    (void)password;
    (void)Clients;
    
    if (tokens.size() < 5) {
        return ERR_NOTENOUGHPARAM(tokens[1]);
    }

    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it) {
        if (it->get_username() == tokens[1]) {
            return ERR_ALREADYREGISTERED(tokens[1]);
        }
    }

    client.set_username(tokens[1]);
    client.set_hostname(tokens[3]);
    client.set_realname(tokens[4]);


    // If we have both NICK and USER, complete registration
    if (client.get_nick() != "" and client.get_username() != "" and client.isRegistered() == false) {
        client.setRegistered(true);
        return sendWelcomeMessage(client);
    }
    return "";
}
