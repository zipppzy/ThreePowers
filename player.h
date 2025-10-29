#ifndef PLAYER_H
#define PLAYER_H

#include <QDebug>

#include "item.h"
#include "location.h"
#include "action.h"
#include "reserve.h"
#include "skill.h"

//TODO: change ptrs to shared_ptr or refrences maybe

class Player
{
public:
    Player();
    Player(unsigned long long int age, unsigned long long int lifespan, std::shared_ptr<Location> currentLocation);
    //TODO: add constructor for toml table
    bool hasSkill(std::string name);
    std::optional<Skill*> findSkill(std::string name);
    auto findSkillIter(std::string name);
    const std::vector<Skill>& getSkills() const;
    bool hasReserve(std::string name);
    std::optional<Reserve*> findReserve(std::string name);
    void addSkillXp(std::string name, double xp);
    void addReserve(const Reserve& reserve);
    bool pickupItem(Item item);
    std::optional<Item*> findItem(Item item);
    std::optional<Item*> findItem(std::string itemName);
    std::shared_ptr<Location> getCurrentLocation();
    void applySkillEffectsCurrentLocation();
    bool startAction(std::shared_ptr<Action> action);       //returns true if action is started and false otherwise
    void savePlayerState(std::string fileName);
    void loadPlayerState(std::string fileName);
    void tick();
    void tick(int numTicks);

    std::vector<int> newSkillIndexes;       //keeps track of new skills for UI updates
    std::vector<int> updatedSkillIndexes;   //keeps track of recently updated skills for UI updates

private:
    bool checkActionRequirements(std::shared_ptr<Action> action) const;
    unsigned long long int age;    //measured in seconds
    unsigned long long int lifespan;

    std::vector<Skill> skills;

    std::vector<Reserve> reserves;

    std::vector<Item> inventory;
    double maxWeight;       //in kg; can't carry above this weight
    double currentWeight;

    std::shared_ptr<Location> currentLocation;
    std::shared_ptr<Action> currentAction;
};

#endif // PLAYER_H
