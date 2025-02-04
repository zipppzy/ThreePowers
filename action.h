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
    Action(toml::table actionTable);

    static void loadActionDatabase(std::string path);
    static std::optional<std::reference_wrapper<const Action>> checkActionDatabaseDatabase(std::string name);

    void reduceDuration(double factor);
    double getCurrentProgress();
    double getDuration();
    const std::map<std::string, double> getSkillRewards();
    const std::vector<Item>& getItemRewards();
    const std::shared_ptr<Requirement> getActionRequirements() const;
    void multiplySkillMultiplier(std::string name, double factor);  //this is awkward maybe should rework
    void copyFrom(const Action& other);
    void reset();       //reset to default values from actionDatabase
    bool tick();        //returns true when action is completed completed; may rework later?

    std::string name;
    bool visibleToPlayer;
private:
    static std::unordered_map<std::string, Action> actionDatabase;      //stores default values for actions

    double baseDuration;        //in seconds
    double duration;
    double currentProgress;

    std::shared_ptr<Requirement> actionRequirements;

    std::map<std::string, std::pair<double, double>> skillRewards;  //key = name of skill  value = (xp, xpMultiplier)
    std::vector<Item> itemRewards;      //includes the count in the Item object
};

#endif // ACTION_H
