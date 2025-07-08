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
    Action(toml::table actionTable);

    static void loadActionDatabase(std::string path);
    static std::optional<std::reference_wrapper<const Action>> checkActionDatabaseDatabase(std::string name);

    void reduceDuration(double factor);
    double getCurrentProgress() const;
    double getDuration() const;
    const std::map<std::string, double> getSkillRewards() const;
    const std::vector<Item>& getItemRewards() const;
    const std::vector<Reserve>& getReserveRewards() const;
    const std::optional<const Requirement*> getActionRequirements() const;
    void multiplySkillMultiplier(std::string name, double factor);  //this is awkward maybe should rework
    void copyFrom(const Action& other);
    void reset();       //reset to default values from actionDatabase
    bool isSuccess() const;
    bool tick();        //returns true when action is completed; may rework later?

    std::string name;
    bool visibleToPlayer;
private:
    static std::unordered_map<std::string, Action> actionDatabase;      //stores default values for actions

    double baseDuration;        //in seconds
    double duration;
    double currentProgress;
    double failureChance;

    std::unique_ptr<Requirement> actionRequirements;

    std::map<std::string, std::pair<double, double>> skillRewards;  //key = name of skill  value = (xp, xpMultiplier)
    std::vector<Item> itemRewards;      //includes the count in the Item object
    std::vector<Reserve> reserveRewards;
};

#endif // ACTION_H
