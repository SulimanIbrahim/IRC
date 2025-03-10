#ifndef BOT_HPP
#define BOT_HPP

#include <vector>
#include <string>
#include <ctime>

#define RPL_BOT_MSG(nickname, username, hostname, channel, message) (":" + nickname + "!~" + username + "@" + hostname + " PRIVMSG "  + channel + " " + message + "\r\n")


class Bot {
private:
    std::vector<std::string> _activities;
    static const size_t MAX_HISTORY = 10;

public:
    Bot();
    ~Bot();
    
    void addActivity(const std::string& activity);
    std::string getResponse(std::string& channel, std::string& nickname, std::string& username, std::string& hostname);    
    std::string getCurrentTime() const;
};

#endif 