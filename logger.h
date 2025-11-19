#ifndef LOGGER_H
#define LOGGER_H

#include <deque>
#include <string>
struct Logger
{
    inline static std::deque<std::string> logMessages;
};

#endif // LOGGER_H
