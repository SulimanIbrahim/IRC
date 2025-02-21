#include "../include/Auth.hpp"

Auth::~Auth() {
    
}

std::string Pass::runAuthCommands(Client &client, const std::vector<std::string>& tokens, std::string password, std::vector<Client> &ignore_Clients) {
    (void)ignore_Clients;
    if (client.isPasswordEntered()) {
        return "\033[1;31m✗ Error: Password has already been entered\n\033[0m";
    }

    if (tokens.size() == 1 || tokens.size() > 2) {
        return "\033[1;36m╔═══════════════════════════════╗\n"
               "║      \033[1;33mPASS Command Usage      \033[1;36m║\n"
               "╚═══════════════════════════════╝\n"
               "\033[1;37mSyntax: \033[1;33mPASS \033[0m<password>\n"
               "\033[0;37mSet your password to authenticate with the server\n\033[0m";
    }

    if (password == tokens[1]) {
        client.setPasswordEntered(true);
        return "\033[1;32m✓ Password accepted! Now use NICK to set your nickname\n\033[0m";
    }

    return "\033[1;31m✗ Error: Password is incorrect\n\033[0m";
}

std::string Nick::runAuthCommands(Client &client, const std::vector<std::string>& tokens, std::string ignore, std::vector<Client> &Clients) {
    (void)ignore;
    if (!client.isPasswordEntered()) {
        return "\033[1;31m✗ Error: You must enter the password first (use PASS command)\n\033[0m";
    }

    if (tokens.size() == 1 || tokens.size() > 2) {
        return "\033[1;36m╔═══════════════════════════════╗\n"
            "║      \033[1;33mNICK Command Usage      \033[1;36m║\n"
            "╚═══════════════════════════════╝\n"
            "\033[1;37mSyntax: \033[1;33mNICK \033[0m<nickname>\n"
            "\033[0;37mSet your nickname (max 9 characters)\n\033[0m";
    }

    std::string nickname = tokens[1];
    if (nickname.empty()) {
        return "\033[1;31m✗ Error: Nickname cannot be empty\n\033[0m";
    }

    if (nickname.size() > 9) {
        return "\033[1;31m✗ Error: Nickname cannot be longer than 9 characters\n\033[0m";
    }

    if (client.get_nick() == nickname) {
        return "\033[1;33mℹ Notice: Nickname is already set to '" + nickname + "'\n\033[0m";
    }
    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it) {
        if (it->get_nick() == nickname) {
            return "\033[1;31m✗ Error: Nickname is already taken\n\033[0m";
        }
    }
    client.set_nick(nickname);
    return "\033[1;32m✓ Nickname set to '" + nickname + "'! Now use USER to set your username\n\033[0m";       
}


std::string User::runAuthCommands(Client &client, const std::vector<std::string>& tokens, std::string ignore, std::vector<Client> &Clients) {
    (void)ignore;
    if (!client.isPasswordEntered()) {
        return "\033[1;31m✗ Error: You must enter the password first (use PASS command)\n\033[0m";
    }

    if (client.get_nick().empty()) {
        return "\033[1;31m✗ Error: You must set a nickname first (use NICK command)\n\033[0m";
    }

    if (tokens.size() == 1 || tokens.size() > 2) {
        return "\033[1;36m╔═══════════════════════════════╗\n"
               "║      \033[1;33mUSER Command Usage      \033[1;36m║\n"
               "╚═══════════════════════════════╝\n"
               "\033[1;37mSyntax: \033[1;33mUSER \033[0m<username>\n"
               "\033[0;37mSet your username (max 9 characters)\n\033[0m";
    }

    std::string username = tokens[1];
    if (username.empty()) {
        return "\033[1;31m✗ Error: Username cannot be empty\n\033[0m";
    }

    if (username.size() > 9) {
        return "\033[1;31m✗ Error: Username cannot be longer than 9 characters\n\033[0m";
    }

    if (client.get_username() == username) {
        return "\033[1;33mℹ Notice: Username is already set to '" + username + "'\n\033[0m";
    }
    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it) {
        if (it->get_username() == username) {
            return "\033[1;31m✗ Error: Username is already taken\n\033[0m";
        }
    }

    client.set_username(username);
    return "\033[1;32m✓ Username set to '" + username + "'! Registration complete! ✨\n\033[0m";
}
