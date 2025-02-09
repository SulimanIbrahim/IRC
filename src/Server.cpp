#include "../include/Server.hpp"

Server::Server(int ac, char **av) : _parser(ac, av) {
    _running = false;
    _port = _parser.getPort();  // Retrieve parsed port
    _password = _parser.getPassword();  // Retrieve parsed password
    _serverSocket = -1;
    std::cout << "Server initialized with port " << _port << " and password " << _password << std::endl;
}

Server::~Server() {
    std::cout << "Server destructor" << std::endl;
    if (_serverSocket != -1)
        close(_serverSocket);
}

std::string Server::ParseComands(std::string str, Client &client) {
    if (str.empty()) {
        return "";
    }
    std::vector<std::string> tokens = _parser.split(str, ' ');
    if (tokens.size() == 0) {
        return "Invalid Syntax\n";
    }
    if (tokens[0] == "pass" && tokens.size() == 2) {
        if (tokens[1] != _password) {
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
    else if (tokens[0] == "nick" && tokens.size() == 2) {
        // parse nickname and check if it already exists and if it is valid
        if (client.get_nick() == tokens[1]) {
            return "Nickname is already set to " + tokens[1] + "\n";
        }
        else if (tokens[1].size() > 9) {
            return "Nickname is too long\n";
        }
        client.set_nick(tokens[1]);
        return "Nickname: " + tokens[1] + "\n";
        // set_nick(tokens[1]);
    }
    else if (tokens[0] == "user" && tokens.size() == 2) {
        // parse username and check if it already exists and if it is valid
        if (client.get_username() == tokens[1]) {
            return "Username is already set to " + tokens[1] + "\n";
        }
        else if (tokens[1].size() > 9) {
            return "Username is too long\n";
        }
        client.set_username(tokens[1]);
        return "Username: " + tokens[1] + "\n";
    }
    else if (tokens[0] == "privmsg" && tokens.size() >= 3) {
        // send a private message to the user token[1]

        return "Private message sent to " + tokens[1] + "\n";
    }
    else if (tokens[0] == "join" && tokens.size() == 2) {
        // join to a channel or create a new one
        for (std::vector<Channel>::iterator it = Channels.begin(); it != Channels.end(); ++it)
        {
            if (it->getChannelName() == tokens[1])
            {
                it->addClient(client);
                client.setChannel(tokens[1]);
                return "Joined channel " + tokens[1] + "\n";
            }
        }
        Channels.push_back(Channel(client, tokens[1]));
        client.setChannel(tokens[1]);
        return "Created and joined channel " + tokens[1] + "\n";
    }
    else if (tokens[0] == "kick" && tokens.size() == 2) {
        // kick a user from a channel
        return "Kicked user " + tokens[1] + " from channel\n";
    }
    else if (tokens[0] == "invite" && tokens.size() == 3) {
        // invite a user to a channel
        return "Invited user " + tokens[1] + " to channel " + tokens[2] + "\n";
    }
    return "Invalid Syntax\n";
}

void Server::CheckComands(std::string str, Client &client) {
    std::string response;
    std::vector<std::string> tokens = _parser.split(str, '\n');
    for (std::vector<std::string>::iterator i = tokens.begin(); i != tokens.end(); ++i)
    {
        response = ParseComands(*i, client);
        send(client.get_pfd().fd, response.c_str(), response.size(), 0);
    }
}

void Server::setupSocket() {
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket == -1) {
        throw std::runtime_error("Failed to create server socket");
    }

    // I added this line to make the server_socket non-blocked
    int flags = fcntl(_serverSocket, F_GETFL, 0);
    fcntl(_serverSocket, F_SETFL, flags | O_NONBLOCK);

    int opt = 1;
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        throw std::runtime_error("Failed to set socket options");
    }
}

void Server::bindSocket() {
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_port = htons(_port);
    _server_addr.sin_addr.s_addr = INADDR_ANY;
    // _server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");;
    if (bind(_serverSocket, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) == -1) {
        throw std::runtime_error("Failed to bind server socket");
    }
}

void Server::listenSocket() {
    if (listen(_serverSocket, 5) == -1) {
        throw std::runtime_error("Failed to listen on server socket");
    }
    std::cout << YELLOW << "Server is listening on port " << RESET << _port << std::endl;
}

void Server::acceptClinets() {
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int client_fd = accept(_serverSocket, (struct sockaddr*)&client_addr, &client_addr_size);
    if (client_fd < 0) {
        if (errno != EWOULDBLOCK) {  // Real error, not just no connections
            throw std::runtime_error("Failed to accept client connection");
        }
        return;
    }
    else {
        std::cout << "client_fd: " << client_fd << std::endl;
        int flags = fcntl(client_fd, F_GETFL, 0);
        fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, INET_ADDRSTRLEN);
        std::cout << CYAN << "Accepted connection from " << RESET << ip << std::endl;
        Clients.push_back(Client(client_fd));
    }
}

void Server::readFromClients() {
    pollfd pfd;
    if (Clients.empty()) {
        return;
    }
    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it) {
        char buffer[1024];
        bzero(buffer, 1024);
        pfd = (*it).get_pfd();
        poll(&pfd, 1, 0);
        if (pfd.revents & POLLIN) {
            int bytes = recv(pfd.fd, buffer, 1024, 0);
            if (bytes <= 0) {
                if (bytes == 0) {
                    std::cout << "Client disconnected " << it->get_username() << std::endl;
                } else {
                    std::cout << "Failed to receive data from client" << std::endl;
                }
                close(pfd.fd);
                Clients.erase(it);
            } else {
                std::cout << "Received " << bytes << " bytes from client " << pfd.fd << std::endl;
                // parse message and check if it is a command
                CheckComands(buffer, *it);
                // std::cout << "Message: " << buffer << std::endl;
            }
        }
    }
}

void Server::start() {
    setupSocket();
    bindSocket();
    listenSocket();
    _running = true;
    std::cout << GREEN << "Server is up and running..." << RESET <<  std::endl;
    while (_running) {
        acceptClinets();
        readFromClients();
    }
}

void Server::stop() {
    _running = false;
    if (_serverSocket != -1)
        close(_serverSocket);
    std::cout << RED << "Server is shutting down..." << RESET << std::endl;
}
