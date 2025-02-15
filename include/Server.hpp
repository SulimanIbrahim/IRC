#ifndef SERVER_HPP
#define SERVER_HPP

//colors 
#define RED "\033[31m"
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"


#include "Client.hpp"
#include "Commands.hpp"
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
#include <sys/event.h>
#include <sys/time.h>
#include <map>

class Server {
private:
    std::string banner;
    bool _running;
    int _port;
    int kq_fd;
    static const int MAX_EVENTS = 32;
    static const int BUFFER_SIZE = 1024;
    std::string _password;
    int _serverSocket;
    std::vector<Client> Clients;
    std::vector<Channel> Channels;
    std::map<std::string, Commands*> _commands;
    struct sockaddr_in _server_addr;
    Parser _parser;

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
    // void monitorClients();
    void registerClientInQueue();
    void handleEvents();
    void handleClientMessage(Client &client);
    void handleDisconnections();
    void processMessage(Client &client, std::string message);
    void setNonBlocking(int fd);
    void stop();
    std::string ParseComands(std::string str, Client &client);
    void CheckComands(std::string str, Client &client);
    // void readFromClients();
    // void writeToClient(Client client, std::string message);
};

#endif
