#ifndef ACTION_H
#define ACTION_H

//Action the player can take
#include <map>
#include <string>

class Action
{
public:
    Action(std::string name, double baseDuration);
    void reduceDuration(double factor);
    const std::map<std::string, double>& getSkillRewards();
    void multiplySkillMultiplier(std::string name, double factor);
    bool tick();        //returns true when action is completed completed; may rework later?
private:
    std::string name;
    double baseDuration;        //in seconds
    double duration;
    double currentProgress;
    std::map<std::string, std::pair<double, double>> skillRewards;  //key = name of skill  value = (xp, xpMultiplier)
};

#endif // ACTION_H
