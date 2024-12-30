#ifndef PLAYER_H
#define PLAYER_H

#include "location.h"
#include "action.h"
#include "skill.h"
#include <unordered_map>

//list of all skills
enum class SkillEnum{
    qiGathering,
    focus
};

class Player
{
public:
    Player();

private:
    unsigned long long int age;    //measured in seconds
    unsigned long long int lifespan;

    std::unordered_map<SkillEnum, Skill> skillLevels;

    double qi;
    double maxQi;

    Location* currentLocation;
    Action* currentAction;
};

#endif // PLAYER_H
