#ifndef PLAYER_H
#define PLAYER_H

#include "location.h"
#include "action.h"
#include "skill.h"
#include "skillnames.h"
#include "toml.hpp"


class Player
{
public:
    Player();
    bool hasSkill(std::string name);
    void savePlayerState();


private:
    unsigned long long int age;    //measured in seconds
    unsigned long long int lifespan;

    std::vector<Skill> skills;

    double qi;
    double maxQi;

    Location* currentLocation;
    Action* currentAction;
};

#endif // PLAYER_H
