#include "../include/Commands.hpp"
#include "../include/Bot.hpp"

std::string BotCommand::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Clients) {
    (void)tokens;
    (void)Channels;
    (void)Clients;

    return client.getBotResponse();
} 