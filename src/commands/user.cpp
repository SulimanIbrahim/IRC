#include "../../include/Commands.hpp"


std::string Commands::User(Client& client, const std::vector<std::string>& tokens) {
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

    client.set_username(username);
    return "\033[1;32m✓ Username set to '" + username + "'! Registration complete! ✨\n\033[0m";
}