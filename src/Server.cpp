#include "../include/Server.hpp"


bool g_running = false;

Server::Server(int ac, char **av) : _parser(ac, av) {
    banner = "\033[1;34m  _   _ _____ _      _      ____ \n | | | | ____| |    | |    |  _  |  \n | |_| |  _| | |    | |    | | | |\n |  _  | |___| |___ | |___ | |_| |\n |_| |_|_____|_____|_____| |_____|\n\n\033[0m";
    _port = _parser.getPort();
    _password = _parser.getPassword();
    _serverSocket = -1;
    kq_fd = -1;
    online_clients = 0;
    _commands["privmsg"] = new Privmsg();
    _commands["join"] = new Join();
    _commands["kick"] = new Kick();
    _commands["sendfile"] = new DCCSend();
    _commands["accept"] = new DCCAccept();
    _commands["reject"] = new DCCReject();
    _commands["pubmsg"] = new Pubmsg();
    _commands["list"] = new List();
    _commands["mode"] = new Mode();
    _commands["invite"] = new Invite();
    _commands["leave"] = new Leave();
    _commands["topic"] = new Topic();
    _commands["cap"] = new Cap();
    _commands["notice"] = new Notice();
    _commands["ping"] = new Ping();
    _auth_commands["pass"] = new Pass();
    _auth_commands["nick"] = new Nick();
    _auth_commands["user"] = new User();
    std::cout << "Server initialized with port " << _port << " and password " << _password << std::endl;
}

Server::~Server() {
    if (_serverSocket != -1)
        close(_serverSocket);
    
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

// static int sendTypingEffect(int client_fd, std::string message, int delay) {
//     int bytes_sent;
//     for (size_t i = 0; i < message.size(); i++) {
//         bytes_sent = send(client_fd, &message[i], 1, 0);
//         usleep(delay * 1000); // Small delay for effect
//     }
//     return bytes_sent;
// }

// static void sendProgressBar(int client_fd) {
//     std::string bar = "\033[1;34mLoading: [";
//     send(client_fd, bar.c_str(), bar.size(), 0);
//     for (int i = 0; i < 20; i++) {
//         send(client_fd, "=", 1, 0);
//         usleep(200000); // 0.3 seconds delay
//     }
//     send(client_fd, "] Done!\n\033[0m", 11, 0);
// }

// static void sendAnimatedText(int client_fd, std::string message) {
//     for (size_t i = 0; i < message.size(); i++) {
//         std::string temp = "\r" + message.substr(0, i+1);
//         send(client_fd, temp.c_str(), temp.size(), 0);
//         usleep(10000); // 0.1 sec delay
//     }
//     send(client_fd, "\n\033[1;32mServer: \033[0m", 20, 0);
// }

std::string Server::ParseComands(std::string str, Client &client) {
    if (str.empty())
        return "";
    std::vector<std::string> tokens = _parser.split(str, ' ');
    std::string cmd = tokens[0];
    for (std::string::iterator it = cmd.begin(); it != cmd.end(); ++it)
        *it = tolower(*it);
    std::cout << "Command: " << cmd << std::endl;
    // Allow CAP negotiation before authentication
    if (cmd == "cap")
        return _commands["cap"]->execute(client, tokens, Channels, Clients);
    if (cmd == "join" && tokens[1] == ":")
        return "Created and joined channel " + tokens[1] + "\n";
    if (_auth_commands.find(cmd) != _auth_commands.end()) {
        return _auth_commands[cmd]->runAuthCommands(client, tokens, _password, Clients);
    }
    if (cmd == "auth")
        return Commands::AuthMsg();
    if (cmd == "help")
        return Commands::Help();
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
    // str = _parser.take_first_line(str);
    for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
        // std::cout << "(" << *it << ")" << std::endl;
        response = ParseComands(*it, client);
        ssize_t bytes_sent = send(client.get_fd(), response.c_str(), response.size(), 0);
        if (bytes_sent < 0) {
            if (errno == EPIPE) {
                std::cerr << "Broken pipe, client disconnected" << std::endl;
                handleDisconnections(client.get_fd());
            } else {
                std::cerr << "Error sending message: " << strerror(errno) << std::endl;
            }
        }
    }
    return;
}

void Server::setupSocket() {
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
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
    // _server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");;
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
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    

    client_fd = accept(_serverSocket, (struct sockaddr*)&client_addr, &client_addr_size);
    if (client_fd < 0) {
        if (errno != EWOULDBLOCK) {
            std::cerr << "Failed to accept client: " << strerror(errno) << std::endl;
        }
        return;
    }
    online_clients++;
    if (online_clients >= 1000) {
        send(client_fd, "\033[1;31m✗ Error: Server is Busy, please try again later\n\033[0m", 50, 0);
        close(client_fd);
        online_clients--;
        return;
    }
    std::cout << "client_fd: " << client_fd << std::endl;
    setNonBlocking(client_fd);
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, ip, INET_ADDRSTRLEN);
    std::cout << CYAN << "Accepted connection from " << RESET << ip << std::endl;
    Client client(client_fd, ip);
    Clients.push_back(client);
    registerClientInQueue();
    // sendProgressBar(client_fd);
    // sendAnimatedText(client_fd, "Welcome to the 500ISE server! Type 'auth' to authenticate\n");
    // std::vector<std::string> tokens = Parser::split("join General", ' ');
    // sendAnimatedText(client_fd, _commands["join"]->execute(client, tokens, Channels, Clients));
    // int bytes_sent = sendTypingEffect(client_fd, banner, 10);
}

void Server::registerServerInQueue() {
    struct kevent server_connection_event;
    EV_SET(&server_connection_event, _serverSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq_fd, &server_connection_event, 1, NULL, 0, NULL) == -1) {
        throw std::runtime_error("Failed to register server socket with kqueue");
    }
    std::cout << "Server registered in kqueue" << std::endl;
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
    std::cout << "Received " << nev << " events" << std::endl;
    std::cout << BLUE <<"Online clients: " << online_clients << RESET << std::endl;
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
                handleDisconnections(events[i].ident);
            }
        }
    }
}

void Server::processMessage(Client &client, std::string message) {
    std::cout << "Received message from client:(" << message << ")"<< std::endl;
    CheckComands(message, client);
}

void Server::handleClientMessage(Client &client) {
    char buffer[BUFFER_SIZE];
    int bytes = recv(client.get_fd(), buffer, BUFFER_SIZE, 0);
    if (bytes < 0) {
        if (errno != EWOULDBLOCK) {
            if (errno == ECONNRESET) {
                handleDisconnections(client.get_fd());
            } else {
                std::cout << "Failed to receive message from client: " << strerror(errno) << std::endl;
            }
        }
    }
    buffer[bytes] = '\0';
    std::string message(buffer);
    // std::replace(message.begin(), message.end(), '\n', ' ');
    std::replace(message.begin(), message.end(), '\r', ' ');
    processMessage(client, message);
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

void Server::start() {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, Server::handlesignal);
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
