#include "../../include/Commands.hpp"

std::string Commands::Pass(Client& client, const std::vector<std::string>& tokens, std::string password) {
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