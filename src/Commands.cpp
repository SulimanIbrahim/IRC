#include "../include/Commands.hpp"

Commands::Commands() {
}

Commands::~Commands() {

}


std::string Commands::Privmsg(Client& client, std::string to, std::string message, std::vector<Channel>& Channels) {
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == client.getChannel()) {
            return it->sendPrivateMessage(message, client, to);
        }
    }
    return "the channel " + client.getChannel() + " does not exist\n";
}

std::string Commands::Join(Client& client, std::string channel, std::vector<Channel>& Channels, std::vector<Client>& Clients) {
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == channel) {
            client.setChannel(channel);
            return it->inviteClient(client, client.get_username(), Clients);
        }
    }
    Channels.push_back(Channel(client, channel));
    client.setChannel(channel);
    return "Created and joined channel " + channel + "\n";
}

std::string Commands::Kick(Client& client, std::string username, std::string from_channel, std::vector<Channel>& Channels) {
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == from_channel) {
            return it->kickClient(client, username);
        }
    }
    return "the channel " + from_channel + " does not exist\n";
}