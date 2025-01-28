#ifndef ACTION_H
#define ACTION_H

//Action the player can take
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include "item.h"
#include "toml.hpp"

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
    void multiplySkillMultiplier(std::string name, double factor);
    bool tick();        //returns true when action is completed completed; may rework later?
private:
    static std::unordered_map<std::string, Action> actionDatabase;
    std::string name;
    double baseDuration;        //in seconds
    double duration;
    double currentProgress;
    std::map<std::string, std::pair<double, double>> skillRewards;  //key = name of skill  value = (xp, xpMultiplier)
    std::vector<Item> itemRewards;      //includes the count in the Item object
};

#endif // ACTION_H
