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
    Player(unsigned long long int age, unsigned long long int lifespan, Location* currentLocation, Action* currentAction);
    bool hasSkill(std::string name);
    void savePlayerState();
    void loadPlayerState();


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
