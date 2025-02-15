#include "../include/Commands.hpp"


Commands::~Commands() {

}


std::string Privmsg::execute(Client &client, std::string to, std::string message, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients) {

        (void)Ignore_Clients;
        for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == client.getChannel()) {
            return it->sendPrivateMessage(message, client, to);
        }
    }
    return "the channel " + client.getChannel() + " does not exist\n";
}

std::string Join::execute(Client &client, std::string channel_name, std::string ignore,std::vector<Channel> &Channels, std::vector<Client> &Clients) {
    (void)ignore;
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == channel_name) {
            client.setChannel(channel_name);
            return it->inviteClient(client, client.get_username(), Clients);
        }
    }
    Channels.push_back(Channel(client, channel_name));
    client.setChannel(channel_name);
    return "Created and joined channel " + channel_name + "\n";
}

std::string Kick::execute(Client &client, std::string username, std::string from_channel, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients){
    (void)Ignore_Clients;    
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == from_channel) {
            return it->kickClient(client, username);
        }
    }
    return "the channel " + from_channel + " does not exist\n";
}



