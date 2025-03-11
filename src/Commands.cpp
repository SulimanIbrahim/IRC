#include "../include/Commands.hpp"
#include "../include/Server.hpp"

Commands::Commands(Server* server) : _server(server) {}

Commands::~Commands() {

}

Privmsg::Privmsg(Server* server) : Commands(server) {}
Topic::Topic(Server* server) : Commands(server) {}
Part::Part(Server* server) : Commands(server) {}
Join::Join(Server* server) : Commands(server) {}
Invite::Invite(Server* server) : Commands(server) {}
Kick::Kick(Server* server) : Commands(server) {}
Mode::Mode(Server* server) : Commands(server) {}
Cap::Cap(Server* server) : Commands(server) {}
Ping::Ping(Server* server) : Commands(server) {}
Notice::Notice(Server* server) : Commands(server) {}

static std::string handleDCCMessage(const std::string& message, Client& client, 
                                  const std::string& recipient, std::vector<Client>& Clients, Server* server) {
    size_t dcc_pos = message.find("DCC SEND");
    if (dcc_pos == std::string::npos) {
        return "Invalid DCC message format\n";
    }
    std::istringstream iss(message.substr(dcc_pos));
    std::string dummy1, dummy2, filename, ip_str, port_str, size_str;
    iss >> dummy1 >> dummy2 >> filename >> ip_str >> port_str >> size_str;

    std::string dcc_message = std::string("\001") + "DCC SEND " + filename + " " + 
                             ip_str + " " + 
                             port_str + " " +
                             size_str + std::string("\001");

    std::string ctcp_message = ":" + client.get_nick() + "!~" + client.get_username() + 
                             "@" + client.get_ip() + " PRIVMSG " + recipient + 
                             " :" + dcc_message + "\r\n";
    
    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it) {
        if (it->get_nick() == recipient) {
            it->addToOutBuffer(ctcp_message);
            if (server) {
                server->enableWriteEvent(it->get_fd());
            }
            return "DCC request sent to " + recipient + "\n";
        }
    }
    return "User " + recipient + " not found\n";
}

std::string Privmsg::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Clients) {
    if (tokens.size() < 3) {
        return BLUE "Privmsg <recipient> <message>\n" RESET;
    }
    
    std::string recipient = tokens[1];
    std::string message = Parser::join_message(tokens);
    
    if (message.find("DCC SEND") != std::string::npos || message.find("\001DCC SEND") != std::string::npos){
        return handleDCCMessage(message, client, recipient, Clients, _server);
    }
    
    // Handle regular private messages to users
    if (recipient[0] != '#') {
        for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it) {
            if (it->get_nick() == recipient) {
                std::string message_to_send = ":" + client.get_nick() + "!~" + client.get_username() + 
                                             "@" + client.get_ip() + " PRIVMSG " + recipient + 
                                             " :" + message + "\r\n";
                it->addToOutBuffer(message_to_send);
                if (_server) {
                    _server->enableWriteEvent(it->get_fd());
                }
                return "Message sent to " + recipient + "\n";
            }
        }
        return "User " + recipient + " not found\n";
    }

    // Handle channel messages
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == recipient) {
            if (client.is_inChannel(recipient)) {
                if (tokens[2] == ":BOT")
                    return client.getBotResponse(recipient);
                it->sendMessage(message, client, Clients);
                return "\n";
            }
            return "You are not in the channel " + recipient + "\n";
        }
    }
    return "The channel " + recipient + " does not exist\n";
}

std::string Part::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Clients) {

    if (tokens.size() < 2) {
        return BLUE "Part <Channel>\n" RESET;
    }
    (void)Clients;
    std::string channel_name = tokens[1];
    if (channel_name == "General") {
        return RED "You are not allowed to leave the General channel\n" RESET;
    }
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == channel_name) {
            if (!client.is_inChannel(channel_name))
                return "You are not in the channel " + channel_name + "\n";
            client.addActivity("Left channel " + channel_name);
            client.removeChannel(channel_name);
            return it->leaveChannel(client);
        }
    }
    return "the channel " + channel_name + " does not exist\n";
}

std::string Join::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Clients) {
    if (tokens.size() != 2 and tokens.size() != 3) {
        return BLUE "Join <Channel> or Join <channel> [password]\n" RESET;
    }
    std::string channel_name = tokens[1];
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == channel_name) {
            if (client.is_inChannel(channel_name))
                return "You are already in the channel " + channel_name + "\n";
            if (it->isPrivate() and !it->isInviteOnly()) {
                if (tokens.size() < 3)
                    return "This channel is private, please provide a password\n";
                else if (it->getPassword() != tokens[2])
                    return "Invalid password\n";
            }
            else if (it->isInviteOnly())
                return "The channel is invite only\n";
            std::string result = it->JoinChannel(client, Clients);
            if (result.find("JOIN") != std::string::npos)
            {
                client.addChannel(channel_name);
                client.addActivity("Joined channel " + tokens[1]);
                it->sendMessage(RPL_JOINMSG(client.get_hostname(), client.get_ip() ,it->getChannelName()), client, Clients);
                return RPL_JOINMSG(client.get_hostname(), client.get_ip() ,it->getChannelName());
            }
            return result;
        }
    }
    if (tokens.size() == 3) {
        Channels.push_back(Channel(client, channel_name, tokens[2], _server));
    } else {
        Channels.push_back(Channel(client, channel_name, _server));
    }
    client.addChannel(channel_name);
    client.addActivity("Created and joined channel " + channel_name);
    return RPL_JOINMSG(client.get_hostname(), client.get_ip() ,channel_name);
}

std::string Invite::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Clients) {
    if (tokens.size() < 3) {
        return BLUE "Invite <nickname> <Channel>\n" RESET;
    }
    std::vector<Client>::iterator client_to_invite;
    std::string channel_name = tokens[2];
    std::string client_to_add = tokens[1];
    if (client_to_add == client.get_nick())
        return "You cannot invite yourself\n";
    if (!client.is_inChannel(channel_name))
        return "You are not in the channel " + channel_name + "\n";
    for (client_to_invite = Clients.begin(); client_to_invite != Clients.end(); ++client_to_invite) {
        if (client_to_invite->get_nick() == client_to_add) {
            break;
        }
    }
    if (client_to_invite == Clients.end())
        return "The client " + client_to_add + " does not exist\n";
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == channel_name) {
            
            std::string result = it->Invite(client, client_to_add, Clients);
            
            if (result.find("joined") != std::string::npos) {
                client.addActivity("Invited " + client_to_add + " to channel " + channel_name);
                for (client_to_invite = Clients.begin(); client_to_invite != Clients.end(); ++client_to_invite) {
                    if (client_to_invite->get_nick() == client_to_add) {
                        client_to_invite->addChannel(channel_name);
                        client_to_invite->addActivity("Invited to channel " + channel_name);
                        break;
                    }
                }
            }
            return result;
        }
    }
    return "the channel " + channel_name + " does not exist\n";
}

std::string Kick::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Clients) {
    std::vector<Client>::iterator client_to_kick;
    if (tokens.size() < 3) {
        return BLUE "Kick <Channel> <nickname>\n" RESET;
    }
    std::string channel_name = tokens[1];
    if (tokens[2] == client.get_nick())
        return "You cannot kick yourself\n";
    if (!client.is_inChannel(channel_name))
        return "You are not in the channel " + channel_name + "\n";
    for (client_to_kick = Clients.begin(); client_to_kick != Clients.end(); ++client_to_kick) {
        if (client_to_kick->get_nick() == tokens[2]) {
            break;
        }
    }
    if (client_to_kick == Clients.end())
        return "The client " + tokens[2] + " does not exist\n";
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == channel_name) {
            std::string result = it->kickClient(client, tokens[2]);
            if (result.find("kicked") != std::string::npos) {
                client_to_kick->removeChannel(channel_name);
                client_to_kick->addActivity(client.get_nick() + " kicked you from channel " + channel_name + "\n");
                client_to_kick->addToOutBuffer(":" + client.get_nick() + " KICK " + channel_name + " " + client_to_kick->get_nick() + "\n");
                if (_server) {
                    _server->enableWriteEvent(client_to_kick->get_fd());
                }
                return result;
            }
            return result;
        }
    }
    return "the channel " + channel_name + " does not exist\n";
}


std::string Topic::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients) {
    (void)Ignore_Clients;
    if (tokens.size() != 3) {
        return BLUE "TOPIC <channel> <topic>: Set the topic of the channel\n" RESET;
    }
    std::string channel = tokens[1];
    if (!client.is_inChannel(channel))
        return "You are not in the channel " + channel + "\n";
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == channel) {
            if (it->isTopicRisterction() and !it->isOperator(client))
                return "You are not an operator in the channel " + channel + "\n";
            it->setTopic(tokens[2]);
            return "Topic set\n";
        }
    }
    return "the channel " + channel + " does not exist\n";
}

std::string Mode::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients) {
    (void)Ignore_Clients;
    if (tokens.size() < 3) {
        return "Usage: MODE <channel> <+/-mode> {args}\n";
    }
    std::string channel = tokens[1];
    if (!client.is_inChannel(channel))
        return "You are not in the channel " + channel + "\n";
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == channel) {
            if (!it->isOperator(client))
                return "You are not an operator in the channel " + channel + "\n";
            return it->mode(client, tokens);
        }
    }
    return "the channel " + channel + " does not exist\n";
}

std::string Cap::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients) {
    (void)Ignore_Clients;
    (void)Channels;
    (void)client;
    
    std::string command = tokens[0];
    for (std::string::iterator it = command.begin(); it != command.end(); ++it) {
        *it = toupper(*it);
    }
    if (command == "CAP") {
        if (tokens.size() >= 2) {
            std::string subcommand = tokens[1];
            for (std::string::iterator it = subcommand.begin(); it != subcommand.end(); ++it) {
                *it = toupper(*it);
            }
            if (subcommand == "LS") {
                return ":" + std::string(SERVER_NAME) + " CAP * LS :multi-prefix\r\n";
            } else if (subcommand == "REQ") {
                return ":" + std::string(SERVER_NAME) + " CAP * ACK :multi-prefix\r\n";
            } else if (subcommand == "END") {
                return "";
            }
        }
    }
    return "";
}

std::string Ping::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients) {
    (void)Ignore_Clients;
    (void)Channels;
    (void)client;
    (void)tokens;
    return "PONG \n";
}

std::string Notice::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients) {
    (void)Ignore_Clients;
    (void)Channels;
    (void)client;
    if (tokens.size() < 2) {
        return "Not enough arguments\n";
    }
    std::string message = Parser::join_message(tokens);
    return "Notice: " + message + "\n";
}

