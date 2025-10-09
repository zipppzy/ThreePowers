#include "action.h"
#include <random>

Action::Action(std::string name, double baseDuration){
    this->name = name;
    this->baseDuration = baseDuration;
    this->duration = baseDuration;
    this->currentProgress = 0;
}

Action::Action(const Action& other)
    : name(other.name),
    visibleToPlayer(other.visibleToPlayer),
    baseDuration(other.baseDuration),
    duration(other.duration),
    currentProgress(other.currentProgress),
    failureChance(other.failureChance),
    actionRequirements(other.actionRequirements
                           ? std::make_unique<Requirement>(*other.actionRequirements)
                           : nullptr),
    skillRewards(other.skillRewards),
    itemRewards(other.itemRewards),
    reserveRewards(other.reserveRewards)
{}

Action& Action::operator=(const Action& other){
    if(this != &other){
        name = other.name;
        visibleToPlayer = other.visibleToPlayer;
        baseDuration = other.baseDuration;
        duration = other.duration;
        currentProgress = other.currentProgress;
        failureChance = other.failureChance;

        // Deep copy the requirement if it exists
        if(other.actionRequirements){
            actionRequirements = std::make_unique<Requirement>(*other.actionRequirements);
        }else{
            actionRequirements.reset();
        }

        skillRewards = other.skillRewards;
        itemRewards = other.itemRewards;
        reserveRewards = other.reserveRewards;
    }

    return *this;
}


Action::Action(toml::table actionTable){
    this->name = actionTable["name"].value_or("");
    this->baseDuration = actionTable["baseDuration"].value_or(-1);
    // if(auto duration = actionTable["duration"].value<double>(); duration){
    //     this->duration = *duration;
    // }else{
    //     this->duration = actionTable["baseDuration"].value_or(-1);
    // }
    this->duration = actionTable["duration"].value_or(this->baseDuration);
    this->currentProgress = actionTable["currentProgress"].value_or(0);
    this->failureChance = actionTable["failureChance"].value_or(0);
    if(auto skillRewardsTable = actionTable["skillRewards"].as_table()){
        for(const auto& [skillName, xp] : *skillRewardsTable){
            double xpFloat = xp.is_floating_point() ? xp.as_floating_point()->get(): static_cast<double>(xp.as_integer()->get());
            this->skillRewards[std::string(skillName)] = xpFloat;
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
                this->reserveRewards.push_back(Reserve(*table, nullptr));
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
                Action::actionDatabase.insert_or_assign(actionTable->at("name").value_or(""), Action(*actionTable));
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
    return this->skillRewards;
}

const std::vector<Item>& Action::getItemRewards() const{
    return this->itemRewards;
}

const std::vector<Reserve>& Action::getReserveRewards() const{
    return this->reserveRewards;
}

const std::optional<const Requirement*> Action::getActionRequirements() const{
    if(this->actionRequirements == nullptr){
        return std::nullopt;
    }
    return this->actionRequirements.get();
}

void Action::applyEffects(const std::vector<Effect>& effects){
    //apply flat effects first
    for(const Effect& effect : effects){
        if(effect.type == Effect::flatDuration){
            this->duration += effect.magnitude;
            continue;
        }

        if(effect.type == Effect::flatFailureChance){
            this->failureChance += effect.magnitude;
            if(this->failureChance < 0.0){
                this->failureChance = 0;
                continue;
            }
            if(this->failureChance > 1.0){
                this->failureChance = 1;
                continue;
            }
            continue;
        }
    }

    //apply mult effects
    for(const Effect& effect : effects){
        if(effect.type == Effect::multDuration){
            this->duration *= effect.magnitude;
            continue;
        }
        if(effect.type == Effect::multFailureChance){
            this->failureChance *= effect.magnitude;
            if(this->failureChance < 0.0){
                this->failureChance = 0;
                continue;
            }
            if(this->failureChance > 1.0){
                this->failureChance = 1;
                continue;
            }
            continue;
        }
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
    this->copyFrom(Action::checkActionDatabaseDatabase(this->name).value());
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
