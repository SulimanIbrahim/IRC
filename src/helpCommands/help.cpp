#include "../../include/Commands.hpp"

std::string Commands::Help() {
    return "\033[1;36m╔═══════════════════════════════════════════╗\n"
           "║        \033[1;33mAvailable IRC Commands        \033[1;36m     ║\n"
           "╚═══════════════════════════════════════════╝\n\n"
           "\033[1;37mAuthentication:\n"
           "  \033[1;33mAUTH\033[0m                 Show authentication steps\n"
           "  \033[1;33mPASS \033[0m<password>      Set your password\n"
           "  \033[1;33mNICK \033[0m<nickname>      Set your nickname\n"
           "  \033[1;33mUSER \033[0m<username>      Set your username\n\n"
           "\033[1;37mChannel Commands:\n"
           "  \033[1;33mJOIN \033[0m<channel>       Join a channel\n"
           "  \033[1;33mKICK \033[0m<user> <chan>   Kick user from channel\n"
           "  \033[1;33mPRIVMSG \033[0m<to> <msg>   Send private message\n"
           "\033[0m";
}