#include "../../include/Commands.hpp"


std::string Commands::Nick(Client& client, const std::vector<std::string>& tokens) {
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

    client.set_nick(nickname);
    return "\033[1;32m✓ Nickname set to '" + nickname + "'! Now use USER to set your username\n\033[0m";
}