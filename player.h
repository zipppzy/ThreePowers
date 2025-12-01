#ifndef PLAYER_H
#define PLAYER_H

#include <QDebug>
#include <deque>


#include "item.h"
#include "location.h"
#include "action.h"
#include "travelaction.h"
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
    void addNewReserve(const Reserve& reserve);
    void addReserve(const std::string& name, double gain);
    const std::vector<Reserve>& getReserves() const;
    void applyActionConstReserveModifiers(std::shared_ptr<Action> action);
    void removeConstActionReserveModifiers(std::shared_ptr<Action> action);
    bool pickupItem(Item item);
    std::optional<Item*> findItem(Item item);
    std::optional<Item*> findItem(std::string itemName);
    std::shared_ptr<Location> getCurrentLocation();
    void applySkillEffectsCurrentLocation();
    void moveLocation(std::shared_ptr<Location> destination);

    const std::vector<std::shared_ptr<Action>>& getGlobalActions() const;
    void addGlobalAction(std::shared_ptr<Action> action);

    const std::deque<std::pair<std::shared_ptr<Action>, int>>& getActionQueue() const;
    void addActionToQueue(std::shared_ptr<Action> action, int numRepeats);
    void removeActionFromQueue(int index, int numRemoved);
    void moveUpActionInQueue(int index);
    void endCurrentAction();
    void attemptStartNextAction();
    void fastForwardQueue();

    bool startAction(std::shared_ptr<Action> action);       //returns true if action is started and false otherwise
    void savePlayerState(std::string fileName);
    void loadPlayerState(std::string fileName);
    void tick();
    void tick(int numTicks);

    std::vector<int> newSkillIndexes;       //keeps track of new skills for UI updates
    std::vector<int> updatedSkillIndexes;   //keeps track of recently updated skills for UI updates
    bool movedLocation = false;


private:
    bool checkActionRequirements(std::shared_ptr<Action> action) const;
    unsigned long long int age;    //measured in seconds
    unsigned long long int lifespan;

    std::vector<Skill> skills;

    std::vector<Reserve> reserves;

    std::vector<Item> inventory;
    double maxWeight;       //in kg; can't carry above this weight
    double currentWeight;

    std::vector<std::shared_ptr<Action>> globalActions;

    //queue of actions with number of times to repeat; -1 means repeat indefinitely
    std::deque<std::pair<std::shared_ptr<Action>, int>> actionQueue;

    std::shared_ptr<Location> currentLocation;
    std::shared_ptr<Action> currentAction;
};

#endif // PLAYER_H
