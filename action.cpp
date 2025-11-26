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
    reserveRewards(other.reserveRewards),
    constantReserveCost(other.constantReserveCost),
    constantReserveGain(other.constantReserveGain)
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
        constantReserveCost = other.constantReserveCost;
        constantReserveGain = other.constantReserveGain;
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
            double xpFloat = xp.is_floating_point() ? xp.as_floating_point()->get() : static_cast<double>(xp.as_integer()->get());
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

    if(auto reserveRewardsTable = actionTable["reserveRewards"].as_table()){
        for(const auto& [reserveName, gain] : *reserveRewardsTable){
            double xpFloat = gain.is_floating_point() ? gain.as_floating_point()->get() : static_cast<double>(gain.as_integer()->get());
            this->skillRewards[std::string(reserveName)] = xpFloat;
        }
    }

    if(auto constantReserveCostTable = actionTable["constantReserveCost"].as_table()){
        for(const auto& [reserveName, cost] : *constantReserveCostTable){
            double costFloat = cost.is_floating_point() ? cost.as_floating_point()->get() : static_cast<double>(cost.as_integer()->get());
            this->constantReserveCost[std::string(reserveName)] = costFloat;
        }
    }

    if(auto constantReserveGainTable = actionTable["constantReserveGain"].as_table()){
        for(const auto& [reserveName, gain] : *constantReserveGainTable){
            double gainFloat = gain.is_floating_point() ? gain.as_floating_point()->get() : static_cast<double>(gain.as_integer()->get());
            this->constantReserveGain[std::string(reserveName)] = gainFloat;
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

const std::map<std::string, double>& Action::getReserveRewards() const{
    return this->reserveRewards;
}

const std::optional<const Requirement*> Action::getActionRequirements() const{
    if(this->actionRequirements == nullptr){
        return std::nullopt;
    }
    return this->actionRequirements.get();
}

const std::map<std::string, double>& Action::getConstantReserveCost() const{
    return this->constantReserveCost;
}
const std::map<std::string, double>& Action::getConstantReserveGain() const{
    return this->constantReserveGain;
}

void Action::applyEffects(const std::vector<std::pair<Effect,int>>& effects){
    //reset action to default values first
    this->reset();

    //apply flat effects first
    // add magnitude * level
    for(const auto& [effect, level] : effects){
        if(effect.type == Effect::flatDuration){
            this->duration += effect.magnitude * level;
            continue;
        }

        if(effect.type == Effect::flatFailureChance){
            this->failureChance += effect.magnitude * level;
            this->failureChance = std::clamp(this->failureChance, 0.0, 1.0);
        }
    }

    //apply mult effects
    //multiply magnitude^level
    for(const auto& [effect,level] : effects){
        if(effect.type == Effect::multDuration){
            this->duration *= pow(effect.magnitude, level);
            continue;
        }
        if(effect.type == Effect::multFailureChance){
            this->failureChance *= pow(effect.magnitude, level);
            this->failureChance = std::clamp(this->failureChance, 0.0, 1.0);
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
    if(auto maybeAction = Action::checkActionDatabaseDatabase(this->name)){
        this->copyFrom(maybeAction.value());
    }
    //TODO: handle reseting TravelActions
}

bool Action::isSuccess() const{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0,1);

    return dist(gen) > failureChance;
}

bool Action::tick(){
    if(baseDuration == -1) return false;
    currentProgress++;
    if(currentProgress > duration){
        currentProgress = 0;
        return true;
    }
    return false;
}
