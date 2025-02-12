#include "../include/Server.hpp"
#include <algorithm>

Server::Server(int ac, char **av) : _parser(ac, av) {
    banner = "Welcome to the IRC Server\nType AUTH to start registration or HELP to see all commands\n";
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
    if (str.empty())
        return "";
    std::vector<std::string> tokens = _parser.split(str, ' ');
    
    // Convert command to lowercase for case-insensitive comparison
    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

    // Always allow AUTH, HELP and PASS commands
    if (cmd == "auth")
        return _commands.Auth();
    if (cmd == "help")
        return _commands.Help();
    if (cmd == "pass")
        return _commands.Pass(client, tokens, _password);

    // Check authentication state for all other commands
    if (!client.isPasswordEntered()) {
        return "\033[1;31m✗ Error: You must enter the password first (use PASS command)\n\033[0m";
    }

    // Check nickname requirement for commands after NICK
    if (cmd != "nick" && client.get_nick().empty()) {
        return "\033[1;31m✗ Error: You must set a nickname first (use NICK command)\n\033[0m";
    }

    // Check username requirement for commands after USER
    if (cmd != "nick" && cmd != "user" && client.get_username().empty()) {
        return "\033[1;31m✗ Error: You must set a username first (use USER command)\n\033[0m";
    }

    // Process commands
    if (cmd == "nick")
        return _commands.Nick(client, tokens);
    
    else if (cmd == "user")
        return _commands.User(client, tokens);

    else if (cmd == "privmsg" && tokens.size() >= 3)
        return _commands.Privmsg(client, tokens[1], tokens[2], Channels);

    else if (cmd == "join" && tokens.size() == 2)
        return _commands.Join(client, tokens[1], Channels, Clients);

    else if (cmd == "kick" && tokens.size() == 3)
        return _commands.Kick(client, tokens[1], tokens[2], Channels);

    else if (cmd == "invite" && tokens.size() == 3)
        // return _commands.Invite(client, tokens[1], tokens[2], Channels);
        return "Invite command not implemented, yet...\n";

    return "\033[1;31m✗ Error: Invalid command. Type HELP to see available commands\n\033[0m";
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
        send(client_fd, banner.c_str(), banner.size(), 0);
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