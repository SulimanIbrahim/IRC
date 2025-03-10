#include "../include/Bot.hpp"
#include <sstream>
#include <iomanip>
#define RED "\033[31m"
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define WHITE "\033[37m"
#define BLACK "\033[30m"
#define BOLD "\033[1m"
#define BOLD_GREEN "\033[1;32m"
#define UNDERLINE "\033[4m"


Bot::Bot() {
}

Bot::~Bot() {
}

void Bot::addActivity(const std::string& activity) {
    std::string timestampedActivity = "[" + getCurrentTime() + "] " + activity;
    
    _activities.push_back(timestampedActivity);
    
    if (_activities.size() > MAX_HISTORY) {
        _activities.erase(_activities.begin());
    }
}

std::string Bot::getCurrentTime() const {
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << timeinfo->tm_hour << ":"
       << std::setfill('0') << std::setw(2) << timeinfo->tm_min << ":"
       << std::setfill('0') << std::setw(2) << timeinfo->tm_sec;
    
    return ss.str();
}

std::string Bot::getResponse(std::string& channel, std::string& nickname, std::string& username, std::string& hostname) {
    std::string response = RPL_BOT_MSG(nickname , username, hostname, channel, YELLOW "Hello " + nickname + "! The current time is " + getCurrentTime() + "\n\n" RESET);
    
    if (_activities.empty()) {
        response += RPL_BOT_MSG(nickname , username, hostname, channel, "You have no recorded activities yet.\r\n");
    } else {
        response += RPL_BOT_MSG(nickname , username, hostname, channel, RESET UNDERLINE "Your recent activities:\r\n" RESET);
        for (size_t i = 0; i < _activities.size(); i++) {
            response += RPL_BOT_MSG(nickname , username, hostname, channel, BOLD_GREEN + _activities[i] + "\r\n" + RESET);
        }
    }
    
    return response;
} 