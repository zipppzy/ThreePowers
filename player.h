#ifndef PLAYER_H
#define PLAYER_H

#include <QDebug>

#include "item.h"
#include "location.h"
#include "action.h"
#include "skill.h"


class Player
{
public:
    Player();
    Player(unsigned long long int age, unsigned long long int lifespan, Location* currentLocation, Action* currentAction);
    bool hasSkill(std::string name);
    std::optional<Skill*> findSkill(std::string name);
    void addSkillXp(std::string name, double xp);
    bool pickupItem(Item item);
    std::optional<Item*> findItem(Item item);
    std::optional<Item*> findItem(std::string itemName);
    void savePlayerState();
    void loadPlayerState();
    void tick();


private:
    bool checkActionRequirements(Action* action) const;
    unsigned long long int age;    //measured in seconds
    unsigned long long int lifespan;

    std::vector<Skill> skills;

    double qi;
    double maxQi;

    std::vector<Item> inventory;
    double maxWeight;       //in kg; can't carry above this weight
    double currentWeight;

    Location* currentLocation;
    Action* currentAction;
};

#endif // PLAYER_H
