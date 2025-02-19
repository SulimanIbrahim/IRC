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
            return it->inviteClient(client, client.get_username(), Clients);
        }
    }
    Channels.push_back(Channel(client, channel_name));
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


std::string DCCSend::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Clients){
    (void)Clients;
    if (tokens.size() < 3) {
        return "Not enough arguments\n";
    }
    std::string file = tokens[1];
    std::string to = tokens[2];
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == client.getChannel()) {
            return it->sendDCCRequest(file, client, to);
        }
    }
    return "DCCSend\n";
}

std::string DCCAccept::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients){
    (void)Ignore_Clients;
    if (tokens.size() < 3) {
        return "Not enough arguments\n";
    }
    std::string client_to_accept = tokens[1];
    std::string filename = tokens[2];
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == client.getChannel()) {
            return it->acceptDCCRequest(client_to_accept, filename);
        }
    }
    return "DCCAccept\n";
}

std::string DCCReject::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients){
    (void)Ignore_Clients;
    if (tokens.size() < 3) {
        return "Not enough arguments\n";
    }
    std::string client_to_reject = tokens[1];
    std::string filename = tokens[2];
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == client.getChannel()) {
            return it->rejectDCCRequest(client_to_reject, filename);
        }
    }
    return "DCCReject\n";
}