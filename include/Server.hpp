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

#define RPL_CREATIONTIME(nickname, channelname, creationtime) ": 329 " + nickname + " #" + channelname + " " + creationtime + "\r\n"
#define RPL_CHANNELMODES(nickname, channelname, modes) ": 324 " + nickname + " #" + channelname + " " + modes + "\r\n"
#define RPL_CHANGEMODE(hostname, channelname, mode, arguments) (":" + hostname + " MODE #" + channelname + " " + mode + " " + arguments + "\r\n")
#define RPL_JOINMSG(hostname, ipaddress, channelname) (":" + hostname + "@" + ipaddress + " JOIN #" + channelname + "\r\n")
#define RPL_NAMREPLY(nickname, channelname, clientslist) (": 353 " + nickname + " @ #" + channelname + " :" + clientslist + "\r\n")
#define RPL_ENDOFNAMES(nickname, channelname) (": 366 " + nickname + " #" + channelname + " :END of /NAMES list" + "\r\n")
#define RPL_TOPICIS(nickname, channelname, topic) (": 332 " + nickname + " #" +channelname + " :" + topic + "\r\n")
#define RPL_MSG(nickname, username, hostname, channel, message) (":" + nickname + "!~" + username + "@" + hostname + "PRIVMSG "  + channel + " :" + message + "\r\n")

///////// ERRORS ////////////////
#define ERR_NEEDMODEPARM(channelname, mode) (": 696 #" + channelname + " * You must specify a parameter for the key mode. " + mode + "\r\n")
#define ERR_INVALIDMODEPARM(channelname, mode) ": 696 #" + channelname + " Invalid mode parameter. " + mode + "\r\n"
#define ERR_KEYSET(channelname) ": 467 #" + channelname + " Channel key already set. " + "\r\n"
#define ERR_UNKNOWNMODE(nickname, channelname, mode) ": 472 " + nickname + " #" + channelname + " " + mode + " :is not a recognised channel mode" + "\r\n"
#define ERR_NOTENOUGHPARAM(nickname) (": 461 " + nickname + " :Not enough parameters." + "\r\n")
#define ERR_CHANNELNOTFOUND(nickname, channelname) (": 403 " + nickname + " " + channelname + " :No such channel" + "\r\n")
#define ERR_NOTOPERATOR(channelname) (": 482 #" + channelname + " :You're not a channel operator" + "\r\n")
#define ERR_NOSUCHNICK(channelname, name) (": 401 #" + channelname + " " + name + " :No such nick/channel" + "\r\n" )
#define ERR_INCORPASS(nickname) (": 464 " + nickname + " :Password incorrect !" + "\r\n" )
#define ERR_ALREADYREGISTERED(nickname) (": 462 " + nickname + " :You may not reregister !" + "\r\n" )
#define ERR_NICKINUSE(nickname) (": 433 " + nickname + " :Nickname is already in use" + "\r\n")
#define ERR_ERRONEUSNICK(nickname) (": 432 " + nickname + " :Erroneus nickname" + "\r\n")
#define ERR_NOTREGISTERED(nickname) (": 451 " + nickname + " :You have not registered!" + "\r\n")
#define ERR_CMDNOTFOUND(nickname, command) (": 421 " + nickname + " " + command + " :Unknown command" + "\r\n")


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
#include <sys/event.h>
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
    std::string ParseComands(std::string str, Client &client);
    void CheckComands(std::string str, Client &client);
    void queueMessageForClient(int client_fd, const std::string& message);
    void enableWriteEvent(int client_fd);
    void handleClientWrite(Client &client);
};

#endif
