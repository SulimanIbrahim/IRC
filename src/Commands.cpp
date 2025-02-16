#include "../include/Commands.hpp"


Commands::~Commands() {

}


std::string Privmsg::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients) {
    (void)Ignore_Clients;
    if (tokens.size() < 3) {
        return "Not enough arguments\n";
    }
    std::string to = tokens[1];
    std::string message = tokens[2];
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == client.getChannel()) {
            return it->sendPrivateMessage(message, client, to);
        }
    }
    return "the channel " + client.getChannel() + " does not exist\n";
}

std::string Join::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Clients) {
    if (tokens.size() < 2) {
        return "Not enough arguments\n";
    }
    std::string channel_name = tokens[1];
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == channel_name) {
            client.setChannel(channel_name);
            if (it->isPrivate())
            {
                if (tokens.size() < 3)
                    return "This channel is private, please provide a password\n";
                else if (it->getPassword() != tokens[2])
                    return "Invalid password\n";
            }
            return it->inviteClient(client, client.get_username(), Clients);
        }
    }
    if (tokens.size() == 3) {
        Channels.push_back(Channel(client, channel_name, tokens[2]));
    } else {
        Channels.push_back(Channel(client, channel_name));
    }
    client.setChannel(channel_name);
    return "Created and joined channel " + channel_name + "\n";
}

std::string Kick::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients){
    (void)Ignore_Clients;
    if (tokens.size() < 3) {
        return "Not enough arguments\n";
    }    
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == tokens[1]) {
            return it->kickClient(client, tokens[2]);
        }
    }
    return "the channel " + tokens[1] + " does not exist\n";
}

std::string Pubmsg::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients) {
    (void)Ignore_Clients;
    if (tokens.size() < 2) {
        return "Not enough arguments\n";
    }
    std::string message = tokens[1];
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == client.getChannel()) {
            it->sendMessage(message, client);
            return "message sent\n";
        }
    }
    return "the channel " + client.getChannel() + " does not exist\n";
}

std::string List::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients) {
    (void)Ignore_Clients;
    (void)tokens;
    std::string list = "Channels:\n";
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == client.getChannel())
            list += it->listClients();
    }
    return list;
}