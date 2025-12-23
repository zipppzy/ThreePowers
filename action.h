#ifndef ACTION_H
#define ACTION_H


#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include "item.h"
#include "requirement.h"
#include "toml.hpp"

//Action the player can take
class Action
{
public:
    Action(std::string name, double baseDuration);
    Action(const Action& other);
    Action& operator=(const Action& other);
    Action(toml::table actionTable);
    virtual ~Action() = default;

    static void loadActionDatabase(std::string path);
    static std::optional<std::reference_wrapper<const Action>> checkActionDatabaseDatabase(std::string name);

    void reduceDuration(double factor);
    double getCurrentProgress() const;
    double getDuration() const;
    const std::map<std::string, double> getSkillRewards() const;
    const std::vector<Item>& getItemRewards() const;
    const std::map<std::string, double>& getReserveRewards() const;
    const std::map<std::string, double>& getConstantReserveCost() const;
    const std::map<std::string, double>& getConstantReserveGain() const;
    const std::optional<const Requirement*> getActionRequirements() const;
    void applyEffects(const std::vector<std::pair<Effect, int>>& effects);
    void copyFrom(const Action& other);
    void reset();       //reset to default values from actionDatabase
    bool isSuccess() const;
    bool tick();        //returns true when action is completed; may rework later?

    std::string name;
    std::string description = "";

    bool visibleToPlayer;
private:
    static std::unordered_map<std::string, Action> actionDatabase;      //stores default values for actions

    // -1 means infinite
    double baseDuration;        //in seconds
    double duration;
    double currentProgress;
    double failureChance = 0.0;

    std::unique_ptr<Requirement> actionRequirements;

    //every tick subtract from these reserves or end action
    //skip action if lacking reserve for one tick
    std::map<std::string, double> constantReserveCost;
    //every tick add to these reserves
    std::map<std::string, double> constantReserveGain;

    std::map<std::string, double> skillRewards;  //key = name of skill  value = (xp, xpMultiplier)
    std::vector<Item> itemRewards;      //includes the count in the Item object
    std::map<std::string, double> reserveRewards;
};

#endif // ACTION_H
