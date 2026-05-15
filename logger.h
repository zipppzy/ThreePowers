#ifndef LOGGER_H
#define LOGGER_H

#include <deque>
#include <string>
struct Logger
{
    inline static std::deque<std::string> logMessages;

    void static log(const std::string& logMessage){
        Logger::logMessages.push_back(logMessage);
    }
};

#endif // LOGGER_H
