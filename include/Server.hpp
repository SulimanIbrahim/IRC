#ifndef SERVER_HPP
#define SERVER_HPP

//colors 
#define RED "\033[31m"
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define WHITE "\033[37m"
#define BLACK "\033[30m"
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"
#define BLINK "\033[5m"
#define REVERSE "\033[7m"
#define HIDDEN "\033[8m"
#define GREY "\033[90m"
#define BOLD_RED "\033[1;31m"
#define BOLD_GREEN "\033[1;32m"
#define BOLD_YELLOW "\033[1;33m"
#define BOLD_BLUE "\033[1;34m"
#define BOLD_MAGENTA "\033[1;35m"
#define BOLD_CYAN "\033[1;36m"
#define BOLD_WHITE "\033[1;37m"
#define BG_BLACK "\033[40m"
#define BG_RED "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_BLUE "\033[44m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"

#include "Client.hpp"
#include "Commands.hpp"
#include "Auth.hpp"
#include "Channel.hpp"
#include "Parser.hpp"
#include <fcntl.h>
#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <poll.h>
#include <algorithm>
#include <signal.h>
#if defined(__APPLE__) && defined(__MACH__)
    #include <sys/event.h>
#else
    #include <kqueue/sys/event.h>
#endif
#include <sys/un.h>
#include <sys/time.h>
#include <map>

#define SERVER_NAME "ft_irc"

class Server {
private:
    std::string banner;
    int _port;
    int online_clients;
    int kq_fd;
    static const int MAX_EVENTS = 32;
    static const int BUFFER_SIZE = 1024;
    std::string _password;
    int _serverSocket;
    std::vector<Client> Clients;
    std::vector<Channel> Channels;
    std::map<std::string, Commands*> _commands;
    std::map<std::string, Auth*> _auth_commands;
    struct sockaddr_in _server_addr;
    Parser _parser;

    // Message queue for centralized message dispatching
    struct PendingMessage {
        int client_fd;
        std::string message;
    };
    std::vector<PendingMessage> _pendingMessages;

public:
    Server(int ac, char **av);
    ~Server();

    void start();
    void setupSocket();
    void bindSocket();
    void listenSocket();
    void acceptClients();
    void initKqueue();
    void registerServerInQueue();
    void registerClientInQueue();
    void handleEvents();
    void handleClientMessage(Client &client);
    void handleDisconnections(int fd);
    void processMessage(Client &client, std::string message);
    void setNonBlocking(int fd);
    static void handlesignal(int sig);
    // void stop();
    std::string ParseComands(std::string str, Client &client);
    void CheckComands(std::string str, Client &client);
    void queueMessageForClient(int client_fd, const std::string& message);
    void enableWriteEvent(int client_fd);
    void handleClientWrite(Client &client);
};

#endif
