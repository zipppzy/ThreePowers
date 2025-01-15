#include "action.h"

Action::Action(std::string name, double baseDuration){
    this->name = name;
    this->baseDuration = baseDuration;
}

void Action::reduceDuration(double factor){
    duration *= factor;
}

std::map<std::string, double> Action::getSkillRewards(){
    std::map<std::string, double> rewards;
    for(const auto& [key, value] : skillRewards){
        rewards[key] = value.first * value.second;
    }
    return rewards;
}

void Action::multiplySkillMultiplier(std::string name, double factor){
    if(auto search = skillRewards.find(name); search != this->skillRewards.end()){
        search->second.second *= factor;
    }
}

bool Action::tick(){
    currentProgress++;
    if(currentProgress > duration){
        currentProgress = 0;
        return true;
    }
    return false;
}
