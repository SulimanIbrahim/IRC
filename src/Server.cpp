#include "../include/Server.hpp"

bool g_running = false;

Server::Server(int ac, char **av) : _parser(ac, av) {
    _port = _parser.getPort();
    _password = _parser.getPassword();
    _serverSocket = -1;
    kq_fd = -1;
    online_clients = 0;
    _commands["privmsg"] = new Privmsg(this);
    _commands["join"] = new Join(this);
    _commands["kick"] = new Kick(this);
    _commands["mode"] = new Mode(this);
    _commands["invite"] = new Invite(this);
    _commands["part"] = new Part(this);
    _commands["topic"] = new Topic(this);
    _commands["cap"] = new Cap(this);
    _commands["notice"] = new Notice(this);
    _commands["ping"] = new Ping(this);
    _auth_commands["pass"] = new Pass();
    _auth_commands["nick"] = new Nick();
    _auth_commands["user"] = new User();
}

Server::~Server() {
    if (_serverSocket != -1)
        close(_serverSocket);
    if (kq_fd != -1)
        close(kq_fd);
    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it) {
        close(it->get_fd());
    }

    for (std::map<std::string, Commands*>::iterator it = _commands.begin(); it != _commands.end(); ++it) {
        delete it->second;
    }
    for (std::map<std::string, Auth*>::iterator it = _auth_commands.begin(); it != _auth_commands.end(); ++it) {
        delete it->second;
    }
    std::cout << MAGENTA << "Server is shutting down..." << RESET << std::endl;
}

std::string Server::ParseComands(std::string str, Client &client) {
    if (str.empty())
        return "";
    std::vector<std::string> tokens = _parser.split(str, ' ');
    std::string cmd = tokens[0];
    for (std::string::iterator it = cmd.begin(); it != cmd.end(); ++it)
        *it = tolower(*it);
    if (cmd == "auth")
        return Commands::AuthMsg();
    if (cmd == "help")
        return Commands::Help();
    if (cmd == "cap")
        return _commands["cap"]->execute(client, tokens, Channels, Clients);
    if (cmd == "ping")
        return _commands["ping"]->execute(client, tokens, Channels, Clients);
    if (cmd == "join" && tokens[1] == ":")
        return "Created and joined channel " + tokens[1] + "\n";
    if (_auth_commands.find(cmd) != _auth_commands.end()) {
        return _auth_commands[cmd]->runAuthCommands(client, tokens, _password, Clients);
    }
    if (!client.isAuthentificated())
        return "\033[1;31m✗ Error: You must authenticate first\n\033[0m";
    if (_commands.find(cmd) != _commands.end()) {
            return _commands[cmd]->execute(client, tokens, Channels, Clients);
    }
    return "\033[1;31m✗ Error: Invalid command. Type HELP to see available commands\n\033[0m";
}

void Server::CheckComands(std::string str, Client &client) {
    std::string response;
    std::vector<std::string> tokens;
    tokens = _parser.split(str, '\n');
    for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
        response = ParseComands(*it, client);
        if (!response.empty()) {
            std::cout << response << std::endl;
            client.addToOutBuffer(response);
            enableWriteEvent(client.get_fd());
        }
    }
}

void Server::setupSocket() {
    _serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_serverSocket == -1) {
        throw std::runtime_error("Failed to create server socket");
    }
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
    if (bind(_serverSocket, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) == -1) {
        throw std::runtime_error("Failed to bind server socket");
    }
}

void Server::listenSocket() {
    if (listen(_serverSocket, SOMAXCONN) == -1) {
        throw std::runtime_error("Failed to listen on server socket");
    }
    std::cout << YELLOW << "Server is listening on port " << RESET << _port << std::endl;
}

void Server::setNonBlocking(int fd) {
    fcntl(fd, F_SETFL, O_NONBLOCK);
}

void Server::registerClientInQueue() {
    int client_fd = Clients.back().get_fd();
    struct kevent events[2];
    
    EV_SET(&events[0], client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    EV_SET(&events[1], client_fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
    
    if (kevent(kq_fd, events, 2, NULL, 0, NULL) < 0) {
        std::cerr << "Failed to register client FD: " << strerror(errno) << std::endl;
    }
}

void Server::acceptClients() {
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    if (online_clients >= 4000) {
        return;
    }
    client_fd = accept(_serverSocket, (struct sockaddr*)&client_addr, &client_addr_size);
    if (client_fd < 0) {
        return;
    }
    online_clients++;
    setNonBlocking(client_fd);
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, ip, INET_ADDRSTRLEN);
    std::cout << CYAN << "Accepted connection from " << RESET << ip << std::endl;
    Client client(client_fd, std::string(ip));
    std::stringstream ss;
    ss << ntohs(client_addr.sin_port);
    client.set_port(ss.str()); // Set the client port using the set_port function
    Clients.push_back(client);
    registerClientInQueue();
}

void Server::registerServerInQueue() {
    struct kevent server_connection_event;
    EV_SET(&server_connection_event, _serverSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq_fd, &server_connection_event, 1, NULL, 0, NULL) == -1) {
        throw std::runtime_error("Failed to register server socket with kqueue");
    }
}

void Server::handleDisconnections(int fd) {
    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it) {
        if (it->get_fd() == fd) {
            close(it->get_fd());
            Clients.erase(it);
            online_clients--;
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
    std::cout << BLUE <<"Online clients: " << online_clients << RESET << std::endl;
    for (int i = 0; i < nev; i++) {
        if (events[i].ident == (unsigned int)_serverSocket) {
            acceptClients();
        }
        else {
            int client_fd = events[i].ident;
            if (events[i].filter == EVFILT_READ) {
                for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it) {
                    if (it->get_fd() == client_fd) {
                        handleClientMessage(*it);
                        break;
                    }
                }
            }
            if (events[i].flags & EV_EOF) {
                handleDisconnections(client_fd);
                continue;
            }
            if (events[i].filter == EVFILT_WRITE) {
                for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it) {
                    if (it->get_fd() == client_fd) {
                        handleClientWrite(*it);
                        break;
                    }
                }
            }
        }
    }
}

void Server::processMessage(Client &client, std::string message) {
    std::string messageWithoutNewline = message;
    if (!message.empty() && message.back() == '\n') {
        messageWithoutNewline.pop_back();
    }
    std::cout << "Received message from client[" << GREEN << client.get_ip() << RESET << "],port[" << YELLOW << client.get_port() << RESET << "]:(" << messageWithoutNewline << ")" << RESET << std::endl;
    CheckComands(message, client);
}

void Server::handleClientMessage(Client &client) {
    char buffer[BUFFER_SIZE];
    int bytes = recv(client.get_fd(), buffer, BUFFER_SIZE, 0);
    if (bytes > 0){
        buffer[bytes] = '\0';
        std::string message(buffer);
        std::replace(message.begin(), message.end(), '\r', ' ');
        processMessage(client, message);
    }
}

void Server::initKqueue() {
    kq_fd = kqueue();
    if (kq_fd == -1) {
        throw std::runtime_error("Failed to create kqueue");
    }
}

void Server::handlesignal(int sig) {
    if (sig == SIGINT) {
        g_running = false;
    }
}

void Server::enableWriteEvent(int client_fd) {
    struct kevent event;
    EV_SET(&event, client_fd, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
    if (kevent(kq_fd, &event, 1, NULL, 0, NULL) < 0) {
        std::cerr << "Failed to enable write event: " << strerror(errno) << std::endl;
    }
}

void Server::handleClientWrite(Client &client) {
    std::string &buffer = client.getOutBuffer();
    struct kevent event;
    if (buffer.empty())
    {
         EV_SET(&event, client.get_fd(), EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
            if (kevent(kq_fd, &event, 1, NULL, 0, NULL) < 0)
                std::cerr << "Error sending message: " << strerror(errno) << std::endl;
        return ;
    }
    ssize_t bytes_sent = send(client.get_fd(), buffer.c_str(), buffer.size(), 0);
    if (bytes_sent >= 0) {
        buffer.erase(0, bytes_sent);
        if (buffer.empty()) {
            EV_SET(&event, client.get_fd(), EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
            if (kevent(kq_fd, &event, 1, NULL, 0, NULL) < 0)
                std::cerr << "Error sending message: " << strerror(errno) << std::endl;
            return;
        }
    } else {
        EV_SET(&event, client.get_fd(), EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
        if (kevent(kq_fd, &event, 1, NULL, 0, NULL) < 0)
            std::cerr << "Error sending message: " << strerror(errno) << std::endl;
    }
}

void Server::start() {
    signal(SIGINT, Server::handlesignal);
    signal(SIGQUIT, Server::handlesignal);
    setupSocket();
    bindSocket();
    listenSocket();
    initKqueue();
    registerServerInQueue();
    g_running = true;
    std::cout << GREEN << "Server is up and running..." << RESET <<  std::endl;
    while (g_running) {
        handleEvents();
    }
}
