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
DCCSend::DCCSend(Server* server) : Commands(server) {}
DCCAccept::DCCAccept(Server* server) : Commands(server) {}
// Pubmsg::Pubmsg(Server* server) : Commands(server) {}
// List::List(Server* server) : Commands(server) {}
DCCReject::DCCReject(Server* server) : Commands(server) {}
Mode::Mode(Server* server) : Commands(server) {}
Cap::Cap(Server* server) : Commands(server) {}
Ping::Ping(Server* server) : Commands(server) {}
Quit::Quit(Server* server) : Commands(server) {}
Notice::Notice(Server* server) : Commands(server) {}
BotCommand::BotCommand(Server* server) : Commands(server) {}

std::string Privmsg::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Clients) {
    (void)client;
    if (tokens.size() < 3) {
        return BLUE "Privmsg <recipient> <message>\n" RESET;
    }
    std::string channel = tokens[1];
    std::string message = Parser::join_message(tokens);
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == channel) {
            if (client.is_inChannel(channel)) {
                it->sendMessage(message, client, Clients);
                return "\n";
            }
            return "You are not in the channel " + channel + "\n";
        }
    }
    return "the channel " + channel + " does not exist\n";
}

std::string Part::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Clients) {

    if (tokens.size() != 2) {
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
            if (result.find("joined") != std::string::npos)
            {
                client.addChannel(channel_name);
                client.addActivity("Joined channel " + tokens[1]);
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
    return "Created and joined channel " + channel_name + "\n";
}

std::string Invite::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Clients) {
    if (tokens.size() != 3) {
        return BLUE "Invite <nickname> <Channel>\n" RESET;
    }
    std::vector<Client>::iterator client_to_invite;
    std::string channel_name = tokens[2];
    std::string client_to_add = tokens[1];
    if (!client.is_inChannel(channel_name))
        return "You are not in the channel " + channel_name + "\n";
    for (client_to_invite = Clients.begin(); client_to_invite != Clients.end(); ++client_to_invite) {
        if (client_to_invite->get_nick() == client_to_add) {
            if (client_to_invite->is_inChannel(channel_name))
                return "The client " + client_to_add + " is already in the channel " + channel_name + "\n";
            else
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

std::string Kick::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients) {
    (void)Ignore_Clients;
    if (tokens.size() != 3) {
        return BLUE "Kick <Channel> <nickname>\n" RESET;
    }
    std::string channel_name = tokens[1];
    if (!client.is_inChannel(channel_name))
        return "You are not in the channel " + channel_name + "\n";
    for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
        if (it->getChannelName() == channel_name) {
            std::string result = it->kickClient(client, tokens[1]);  
            if (result.find("kicked") != std::string::npos) {
                client.addActivity("Kicked " + tokens[1] + " from channel " + channel_name);
                for (std::vector<Client>::iterator client_to_kick = Ignore_Clients.begin(); client_to_kick != Ignore_Clients.end(); ++client_to_kick) {
                    if (client_to_kick->get_nick() == tokens[2]) {
                        client_to_kick->removeChannel(channel_name);
                        client_to_kick->addActivity("Kicked from channel " + channel_name);
                        break;
                    }
                }
            }
            return result;
        }
    }
    return "the channel " + channel_name + " does not exist\n";
}

// std::string Pubmsg::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients) {
//     (void)Ignore_Clients;
//     (void)client;
//     (void)tokens;
//     (void)Channels;
//     // if (tokens.size() != 2) {
//         return BLUE "Pubmsg <message>\n" RESET;
//     // }
//     // if (client.getChannel() == "General") {
//     //     return "You can't send a public message in the General channel\n";
//     // }
//     // std::string message = Parser::join_message(tokens);
//     // for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
//     //     if (it->getChannelName() == client.getChannel()) {
//     //         it->sendMessage(message, client);
//     //         return "message sent\n";
//     //     }
//     // }
//     // return "the channel " + client.getChannel() + " does not exist\n";
// }

// std::string List::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients) {
//     (void)Ignore_Clients;
//     (void)tokens;
//     if (client.getChannel() == "General") {
//         return "You can't list the clients in the General channel\n";
//     }
//     std::string list = "Channels:\n";
//     for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
//         if (it->getChannelName() == client.getChannel())
//             list += it->listClients();
//     }
//     return list;
// }

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
    if (tokens.size() < 4) {
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


std::string DCCSend::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Clients) {
    (void)Clients;
    
    if (tokens.size() < 3) {
        return "Usage: SENDFILE <filename> <recipient>\n";
    }
    
    std::string filename = tokens[1];
    std::string recipient = tokens[2];
    
    // Check if file exists
    FILE *file = fopen(filename.c_str(), "rb");
    if (!file) {
        return "Error: File " + filename + " not found\n";
    }
    fclose(file);
    
    // i change the way clients are storng which channel they are in, so...

    (void)Channels;
    (void)client;
    // for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it) {
    //     if (it->getChannelName() == client.getChannel()) {
    //         return it->sendDCCRequest(filename, client, recipient);
    //     }
    // }
    
    return "Error: You are not in a channel\n";
}

std::string DCCAccept::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients) {
    (void)Ignore_Clients;
    (void)Channels;
    (void)client;
    (void)tokens;
    
    // DCC Accept is now handled directly by the client
    return "DCC file transfers are handled directly between clients. Your IRC client should automatically handle incoming transfers.\n";
}

std::string DCCReject::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients) {
    (void)Ignore_Clients;
    (void)Channels;
    (void)client;
    (void)tokens;
    
    // DCC Reject is now handled directly by the client
    return "DCC file transfers are handled directly between clients. Use your IRC client's commands to reject transfers.\n";
}

std::string Ping::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients) {
    (void)Ignore_Clients;
    (void)Channels;
    // if (tokens.size() > 1){
    //     std::cerr << "PING command requires at least 1 argument\n";
    // }
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

std::string Quit::execute(Client &client, std::vector<std::string> &tokens, std::vector<Channel> &Channels, std::vector<Client> &Ignore_Clients) {
    (void)Ignore_Clients;
    (void)client;
    (void)Channels;
    (void)tokens;
    return "Goodbye\n";
}

// std::string Commands::Help() {
//     std::string help = "\033[1;32m";
//     help += "Available commands:\n";
//     help += "JOIN <channel> [password] - Join a channel\n";
//     help += "LEAVE - Leave the current channel\n";
//     help += "PRIVMSG <recipient> <message> - Send a private message\n";
//     help += "PUBMSG <message> - Send a message to the channel\n";
//     help += "KICK <user> - Kick a user from the channel (operators only)\n";
//     help += "INVITE <user> - Invite a user to the channel (operators only)\n";
//     help += "TOPIC - View the channel topic\n";
//     help += "LIST - List users in the channel\n";
//     help += "MODE <mode> [args] - Set channel modes (operators only)\n";
//     help += "  +i/-i - Set/unset invite-only mode\n";
//     help += "  +t/-t - Set/unset topic restriction mode\n";
//     help += "  +k/-k <password> - Set/unset channel password\n";
//     help += "  +o/-o <user> - Give/take operator status\n";
//     help += "SENDFILE <filename> <recipient> - Send a file via DCC\n";
//     help += "ACCEPT <sender> <filename> - Accept a DCC file transfer\n";
//     help += "REJECT <sender> <filename> - Reject a DCC file transfer\n";
//     help += "AUTH - Show authentication commands\n";
//     help += "\033[0m";
//     return help;
// }