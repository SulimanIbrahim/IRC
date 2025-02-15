
#include "../../include/Commands.hpp"

std::string Commands::AuthMsg() {
    return "\033[1;36m╔════════════════════════════════════════╗\n"
           "║     \033[1;33mWelcome to IRC Registration     \033[1;36m   ║\n"
           "╚════════════════════════════════════════╝\n\n"
           "\033[1;37mFollow these steps to connect:\n\n"
           "\033[1;32m1. \033[1;37mSet your password:\n"
           "   \033[1;33mPASS \033[0m<your-password>\n"
           "\033[1;32m2. \033[1;37mChoose your nickname:\n"
           "   \033[1;33mNICK \033[0m<your-nickname>\n"
           "\033[1;32m3. \033[1;37mSet your username:\n"
           "   \033[1;33mUSER \033[0m<your-username>\n"
           "\033[1;36m✨ \033[1;37mOnce completed, you'll be registered!\n"
           "\033[0m";
}