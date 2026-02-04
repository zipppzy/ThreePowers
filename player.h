#ifndef PLAYER_H
#define PLAYER_H

#include <QDebug>
#include <deque>
#include <set>

#include "item.h"
#include "location.h"
#include "action.h"
#include "travelaction.h"
#include "reserve.h"
#include "skill.h"
#include "triggermanager.h"
#include "instanteffect.h"
#include "world.h"

//TODO: change ptrs to shared_ptr or refrences maybe

class Player
{
public:
    Player();
    Player(unsigned long long int age, unsigned long long int lifespan, std::shared_ptr<Location> currentLocation, World* world);
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
    const std::vector<Item>& getInventory() const;
    std::shared_ptr<Location> getCurrentLocation();
    void applySkillEffectsCurrentLocation();
    void moveLocation(std::shared_ptr<Location> destination);

    void unlockLocation(const std::string& locationName);
    void lockLocation(const std::string& locationName);
    void unhideLocation(const std::string& locationName);
    void hideLocation(const std::string& locationName);

    void applyInstantEffect(const InstantEffect& effect);

    void queueEvent(const std::string& eventId);

    const std::vector<std::shared_ptr<Action>>& getGlobalActions() const;
    void addGlobalAction(std::shared_ptr<Action> action);

    void setRestAction(std::shared_ptr<Action> action);
    const std::shared_ptr<Action> getRestAction() const;
    void handleAutoRest(Reserve* targetReserve);
    bool autoRest = true;
    bool isAutoResting = false;
    Reserve* targetReserve = nullptr;

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

    TriggerManager triggerManager;

    std::vector<int> newSkillIndexes;       //keeps track of new skills for UI updates
    std::vector<int> updatedSkillIndexes;   //keeps track of recently updated skills for UI updates
    bool movedLocation = false;
    bool inventoryChanged = false;

    // Queue of event IDs to be displayed
    std::deque<std::string> pendingEvents;


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

    std::shared_ptr<Action> restAction = nullptr;

    //queue of actions with number of times to repeat; -1 means repeat indefinitely
    std::deque<std::pair<std::shared_ptr<Action>, int>> actionQueue;

    World* world = nullptr;

    std::shared_ptr<Location> currentLocation;
    std::shared_ptr<Action> currentAction;

    std::set<std::string> visitedLocations;

    TriggerContext triggerContext;
};

#endif // PLAYER_H
