#include "action.h"
#include <random>

Action::Action(std::string name, double baseDuration){
    this->name = name;
    this->baseDuration = baseDuration;
    this->duration = baseDuration;
    this->currentProgress = 0;
}

Action::Action(toml::table actionTable){
    this->name = actionTable["name"].value_or("");
    this->baseDuration = actionTable["baseDuration"].value_or(-1);
    if(auto duration = actionTable["duration"].value<double>(); duration){
        this->duration = *duration;
    }else{
        this->duration = actionTable["baseDuration"].value_or(-1);
    }
    this->currentProgress = actionTable["currentProgress"].value_or(0);

    if(auto skillRewardsTable = actionTable["skillRewards"].as_table()){
        for(const auto& [key, value] : *skillRewardsTable){
            if (const auto* arr = value.as_array(); arr && arr->size() == 2){
                Action::skillRewards[std::string(key)] = {arr->get(0)->value_or(0.0), arr->get(1)->value_or(1.0)};
            }
        }
    }

    if(auto itemRewardsArray = actionTable["itemRewards"].as_array()){
        for(const auto& itemTable : *itemRewardsArray){
            if(const auto* table = itemTable.as_table()){
                this->itemRewards.push_back(Item(*table));
            }
        }
    }
    if(auto reserveRewardsArray = actionTable["reserveRewards"].as_array()){
        for(const auto& reserveTable : *reserveRewardsArray){
            if(const auto* table = reserveTable.as_table()){
                this->reserveRewards.push_back(Reserve(*table));
            }
        }
    }
    this->visibleToPlayer = false;
}

std::unordered_map<std::string, Action> Action::actionDatabase;

void Action::loadActionDatabase(std::string path){
    auto in = toml::parse_file(path);

    Action::actionDatabase.clear();
    if(auto actionsArray = in["actions"].as_array()){
        for(auto& elem : *actionsArray){
            if(auto actionTable = elem.as_table()){
                Action::actionDatabase[actionTable->at("name").value_or("")] = Action(*actionTable);
            }
        }
    }
}

std::optional<std::reference_wrapper<const Action>> Action::checkActionDatabaseDatabase(std::string name){
    if(auto search = Action::actionDatabase.find(name); search != Action::actionDatabase.end()){
        return std::cref(search->second);
    }else{
        return std::nullopt;
    }
}

void Action::reduceDuration(double factor){
    duration *= factor;
}

double Action::getCurrentProgress() const{
    return this->currentProgress;
}

double Action::getDuration() const{
    return this->duration;
}

const std::map<std::string, double> Action::getSkillRewards() const{
    std::map<std::string, double> rewards;
    for(const auto& [key, value] : skillRewards){
        rewards[key] = value.first * value.second;
    }
    return rewards;
}

const std::vector<Item>& Action::getItemRewards() const{
    return this->itemRewards;
}

const std::vector<Reserve> Action::getReserveRewards() const{
    return this->reserveRewards;
}

const std::shared_ptr<Requirement> Action::getActionRequirements() const{
    return this->actionRequirements;
}

void Action::multiplySkillMultiplier(std::string name, double factor){
    if(auto search = skillRewards.find(name); search != this->skillRewards.end()){
        search->second.second *= factor;
    }
}

void Action::copyFrom(const Action& other){
    this->name = other.name;
    this->baseDuration = other.baseDuration;
    this->duration = other.duration;
    this->currentProgress = other.currentProgress;
    this->skillRewards = other.skillRewards;
    this->itemRewards = other.itemRewards;

}

void Action::reset(){
    this->copyFrom(Action::actionDatabase[this->name]);
}

bool Action::isSuccess() const{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0,1);

    return dist(gen) > failureChance;
}

bool Action::tick(){
    currentProgress++;
    if(currentProgress > duration){
        currentProgress = 0;
        return true;
    }
    return false;
}
