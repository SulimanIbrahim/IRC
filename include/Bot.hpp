#ifndef BOT_HPP
#define BOT_HPP

#include <vector>
#include <string>
#include <ctime>

class Bot {
private:
    std::vector<std::string> _activities;
    static const size_t MAX_HISTORY = 10;

public:
    Bot();
    ~Bot();
    
    void addActivity(const std::string& activity);
    std::string getResponse(const std::string& username);    
    std::string getCurrentTime() const;
};

#endif 