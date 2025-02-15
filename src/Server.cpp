#include "../include/Server.hpp"

Server::Server(int ac, char **av) : _parser(ac, av) {
    banner = "Welcome to the IRC Server\n";
    _running = false;
    _port = _parser.getPort();  // Retrieve parsed port
    _password = _parser.getPassword();  // Retrieve parsed password
    _serverSocket = -1;
    kq_fd = -1;
    _commands["privmsg"] = new Privmsg();
    _commands["join"] = new Join();
    _commands["kick"] = new Kick();
    _auth_commands["pass"] = new Pass();
    _auth_commands["nick"] = new Nick();
    _auth_commands["user"] = new User();
    std::cout << "Server initialized with port " << _port << " and password " << _password << std::endl;
}

Server::~Server() {
    std::cout << "Server destructor" << std::endl;
    if (_serverSocket != -1)
        close(_serverSocket);
    
    for (std::map<std::string, Commands*>::iterator it = _commands.begin(); it != _commands.end(); ++it) {
        delete it->second;
    }
    for (std::map<std::string, Auth*>::iterator it = _auth_commands.begin(); it != _auth_commands.end(); ++it) {
        delete it->second;
    }
}

std::string Server::ParseComands(std::string str, Client &client) {
    if (str.empty())
        return "";
    std::vector<std::string> tokens = _parser.split(str, ' ');
    // Convert command to lowercase for case-insensitive comparison
    std::string cmd = tokens[0];
    for (std::string::iterator it = cmd.begin(); it != cmd.end(); ++it)
        *it = tolower(*it);
    if (cmd == "auth")
        return Commands::AuthMsg();
    if (cmd == "help")
        return Commands::Help();
    // polymorphism for commands Kick Join and Privmsg
    if (_commands.find(cmd) != _commands.end()) {
        if (client.isPasswordEntered() or cmd == "pass")
            return _commands[cmd]->execute(client, tokens, Channels, Clients);
        else
            return "\033[1;31m✗ Error: Password has not been entered\n\033[0m";
    }
    // polymorphism for Auth commands Pass Nick and User
    if (_auth_commands.find(cmd) != _auth_commands.end()) {
        return _auth_commands[cmd]->runAuthCommands(client, tokens, _password);
    }
    return "\033[1;31m✗ Error: Invalid command. Type HELP to see available commands\n\033[0m";
}

void Server::CheckComands(std::string str, Client &client) {
    std::string response;
    std::vector<std::string> tokens = _parser.split(str, '\n');
    for (std::vector<std::string>::iterator i = tokens.begin(); i != tokens.end(); ++i)
    {
        response = ParseComands(*i, client);
        send(client.get_fd(), response.c_str(), response.size(), 0);
    }
}

void Server::setupSocket() {
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket == -1) {
        throw std::runtime_error("Failed to create server socket");
    }

    // I added this line to make the server_socket non-blocked
    setNonBlocking(_serverSocket);

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

void Server::setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void Server::registerClientInQueue() {
    int client_fd = Clients.back().get_fd();
    struct kevent event;
    EV_SET(&event, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq_fd, &event, 1, NULL, 0, NULL) < 0) {
        std::cerr << "Failed to register client FD: " << strerror(errno) << std::endl;
    }
}

void Server::acceptClients() {
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int client_fd = accept(_serverSocket, (struct sockaddr*)&client_addr, &client_addr_size);
    if (client_fd < 0) {
        if (errno != EWOULDBLOCK) {
            throw std::runtime_error("Failed to accept client connection");
        }
        return;
    } else {
        std::cout << "client_fd: " << client_fd << std::endl;
        setNonBlocking(client_fd);
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, INET_ADDRSTRLEN);
        std::cout << CYAN << "Accepted connection from " << RESET << ip << std::endl;
        Clients.push_back(Client(client_fd));
        registerClientInQueue();
        send(client_fd, banner.c_str(), banner.size(), 0);
    }
}

void Server::registerServerInQueue() {
    struct kevent server_connection_event;
    EV_SET(&server_connection_event, _serverSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq_fd, &server_connection_event, 1, NULL, 0, NULL) == -1) {
        throw std::runtime_error("Failed to register server socket with kqueue");
    }
    std::cout << "Server registered in kqueue" << std::endl;
}

void Server::handleDisconnections() {
    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it) {
        if (it->get_fd() & EV_EOF) {
            close(it->get_fd());
            Clients.erase(it);
            std::cout << RED << "Client disconnected" << RESET << std::endl;
            break;
        }
    }
}

void Server::handleEvents() {
    struct kevent events[MAX_EVENTS];
    int nev = kevent(kq_fd, NULL, 0, events, MAX_EVENTS, NULL);
    if (nev == -1) {
        throw std::runtime_error("Failed to poll events from kqueue");
    }
    for (int i = 0; i < nev; i++) {
        if (events[i].ident == (unsigned int)_serverSocket) {
            acceptClients();
        }  
        else {
            if (events[i].flags & EVFILT_READ) {
                for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it) {
                    if ((unsigned int)it->get_fd() == events[i].ident) {
                        handleClientMessage(*it);
                        break;
                    }
                }
            }
            if (events[i].flags & EV_EOF) {
                handleDisconnections();
            }
        }
    }
}

void Server::processMessage(Client &client, std::string message) {
    std::cout << "Received message from client: " << message << std::endl;
    CheckComands(message, client);
}

void Server::handleClientMessage(Client &client) {
    char buffer[BUFFER_SIZE];
    int bytes = recv(client.get_fd(), buffer, BUFFER_SIZE, 0);
    if (bytes == -1) {
        throw std::runtime_error("Failed to receive message from client");
    }
    else if (bytes == 0) {
        return;
    }
    buffer[bytes] = '\0';
    std::string message(buffer);
    processMessage(client, message);
}


void Server::initKqueue() {
    kq_fd = kqueue();
    if (kq_fd == -1) {
        throw std::runtime_error("Failed to create kqueue");
    }
}

void Server::start() {
    setupSocket();
    bindSocket();
    listenSocket();
    initKqueue();
    registerServerInQueue();
    _running = true;
    std::cout << GREEN << "Server is up and running..." << RESET <<  std::endl;
    while (_running) {
        handleEvents();
    }
}


void Server::stop() {
    _running = false;
    if (_serverSocket != -1)
        close(_serverSocket);
    std::cout << RED << "Server is shutting down..." << RESET << std::endl;
}