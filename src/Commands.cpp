#include "../include/Commands.hpp"

Commands::Commands() {
}

Commands::~Commands() {

}

std::string Commands::Join(Client& client, std::string ChannelName) {
    if (ChannelName == client.getChannel()) {
        return "Already in channel " + ChannelName + "\n";
    }
    client.setChannel(ChannelName);
    return "Joined channel " + ChannelName + "\n";
}

std::string Commands::Pass(Client& client, std::string enterd_password, std::string password) {
    if (password != enterd_password) {
        if (client.isPasswordEntered() == false) {
            client.setPasswordEntered(true);
            return "Password entered\n";
        }
        else {
            return "Password is already entered\n";
        }
    }
    return "Password is incorrect\n";
}

std::string Commands::Nick(Client& client, std::string nickname) {
    if (client.get_nick() == nickname) {
        return "Nickname is already set to " + nickname + "\n";
    }
    else if (nickname.size() > 9) {
        return "Nickname is too long\n";
    }
    client.set_nick(nickname);
    return "Nickname: " + nickname + "\n";
}

std::string Commands::User(Client& client, std::string username) {
    if (client.get_username() == username) {
        return "Username is already set to " + username + "\n";
    }
    else if (username.size() > 9) {
        return "Username is too long\n";
    }
    client.set_username(username);
    return "Username: " + username + "\n";
}

std::string Commands::Privmsg(Client& client, std::string to, std::string message, std::vector<Channel>& Channels) {
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == client.getChannel()) {
            return it->sendPrivateMessage(message, client, to);
        }
    }
    return "You are not in a channel\n";
}

std::string Commands::Join(Client& client, std::string channel, std::vector<Channel>& Channels, std::vector<Client>& Clients) {
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == channel) {
            client.setChannel(channel);
            return it->addClient(client, client.get_username(), Clients);
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
    return "You are not in a channel\n";
}