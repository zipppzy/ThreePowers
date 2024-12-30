#ifndef PLAYER_H
#define PLAYER_H

#include "location.h"
#include "action.h"
#include "skill.h"
#include "skillnames.h"
#include "toml.hpp"
#include <unordered_map>


class Player
{
public:
    Player();

private:
    unsigned long long int age;    //measured in seconds
    unsigned long long int lifespan;

    std::unordered_map<SkillName, Skill> skillLevels;

    double qi;
    double maxQi;

    Location* currentLocation;
    Action* currentAction;
};

#endif // PLAYER_H
