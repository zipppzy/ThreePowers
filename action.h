#ifndef ACTION_H
#define ACTION_H

//Abstract class
//Action the player can take
#include <string>

class Action
{
public:
    Action();
private:
    std::string name;
    double baseDuration;
};

#endif // ACTION_H
