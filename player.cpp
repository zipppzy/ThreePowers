#include "player.h"


Player::Player() {
}

Player::Player(unsigned long long int age, unsigned long long int lifespan, std::shared_ptr<Location> currentLocation){
    this->age = age;
    this->lifespan = lifespan;
    this->currentLocation = currentLocation;
    this->maxWeight = 50;
    this->currentWeight = 0;
}

bool Player::hasSkill(std::string name){
    return this->findSkill(name) ? true : false;
}

std::optional<Skill*> Player::findSkill(std::string name){
    auto it  = std::find_if(this->skills.begin(), this->skills.end(), [&name](const Skill& skill){return skill.name == name;});
    if(it == this->skills.end()){
        return std::nullopt;
    }else{
        return &(*it);
    }
}

bool Player::hasReserve(std::string name){
    return this->findReserve(name) ? true : false;
}

std::optional<Reserve*> Player::findReserve(std::string name){
    auto it  = std::find_if(this->reserves.begin(), this->reserves.end(), [&name](const Reserve& reserve){return reserve.name == name;});
    if(it == this->reserves.end()){
        return std::nullopt;
    }else{
        return &(*it);
    }
}

void Player::addSkillXp(std::string name, double xp){
    if(auto maybeSkill = this->findSkill(name)){
        maybeSkill.value()->addXp(xp);
    }else{
        auto result = Skill::checkSkillDatabase(name);
        if(result){
            this->skills.push_back(Skill(result->get()));
        }else{
            qDebug() << "Tried to add xp to non-valid skill";
        }
    }
}

void Player::addReserve(const Reserve& reserve){
    auto existingReserve = this->findReserve(reserve.name);
    if(existingReserve){
        (*existingReserve)->add(reserve.getCurrentValue());
    }else{
        this->reserves.push_back(Reserve(reserve));
    }
}

bool Player::pickupItem(Item item){
    if(this->currentWeight+(item.getWeight()*item.count) > this->maxWeight){
        return false;
    }else{
        //if there is already an existing item with the same name then add to that stack
        if(auto existingItem = this->findItem(item.name); existingItem){
            (*existingItem)->count += item.count;
        }else{
            inventory.push_back(item);
        }
        this->currentWeight = item.getWeight()*item.count;
        return true;
    }
}

std::optional<Item*> Player::findItem(std::string itemName){
    auto it  = std::find_if(this->inventory.begin(), this->inventory.end(), [&itemName](const Item& item){return item.name ==itemName;});
    if(it == inventory.end()){
        return std::nullopt;
    }else{
        return &(*it);
    }
}

std::optional<Item*> Player::findItem(Item item){
    return findItem(item.name);
}

std::shared_ptr<Location> Player::getCurrentLocation(){
    return currentLocation;
}

void Player::applySkillEffectsCurrentLocation(){
    for(auto& action : currentLocation->getActions()){
        std::string actionName = action->name;
        std::vector<std::pair<Effect,int>> skillEffectList;

        for(const auto& skill : skills){
            if(auto maybeEffects = skill.checkEffects(actionName)){
                for (const auto& effect : maybeEffects->get()) {
                    skillEffectList.emplace_back(effect, skill.getLevel());
                }
            }
        }
        action->applyEffects(skillEffectList);
    }
}

bool Player::startAction(std::shared_ptr<Action> action){
    if(checkActionRequirements(action)){
        this->currentAction = action;
        return true;
    }else{
        return false;
    }
}
void Player::savePlayerState(std::string fileName){
    toml::table playerData;

    toml::table attributes;
    attributes.insert("age", std::to_string(age));              //stored as string to avoid length limit
    attributes.insert("lifespan", std::to_string(lifespan));    //stored as string to avoid length limit

    playerData.insert("attributes", std::move(attributes));

    toml::array skills;
    for(Skill& i : this->skills){
        skills.push_back(i.getSkillTable());
    }

    playerData.insert("skills", std::move(skills));

    std::ofstream file(fileName);
    file << playerData;
    file.close();
}

void Player::loadPlayerState(std::string fileName){
    auto playerData = toml::parse_file(fileName);

    auto attributes = playerData["attributes"];
    this->age = std::stoull(playerData["age"].value_or("0"));
    this->lifespan = std::stoull(playerData["lifespan"].value_or("0"));

    this->skills.clear();  // Clear existing skills before loading
    if(auto skillsArray = playerData["skills"].as_array()){
        for(const auto& elem : *skillsArray){
            if(auto skillTable = elem.as_table()){
                this->skills.push_back(Skill(*skillTable));
            }
        }
    }
}

void Player::tick(){
    this->age++;
    if(currentAction != nullptr){
        //ticks currentAction and check if action is completed then gives rewards
        if(currentAction->tick()){
            if(currentAction->isSuccess()){
                auto skillRewards = currentAction->getSkillRewards();
                for(const auto& [key,value] : skillRewards){
                    this->addSkillXp(key, value);
                }
                auto itemRewards = currentAction->getItemRewards();
                for(const auto& item : itemRewards){
                    if(!this->pickupItem(item)){
                        qDebug()<<"Couldn't pickup item";
                    }
                }
                auto reserveRewards = currentAction->getReserveRewards();
                for(const Reserve& reserve : reserveRewards){
                    this->addReserve(reserve);
                }
            }else{
                qDebug("Action Failed");
            }
        }
    }
}

bool Player::checkActionRequirements(std::shared_ptr<Action> action) const{
    auto maybeActionRequirement = action->getActionRequirements();
    if(!maybeActionRequirement){
        return true;
    }
    return maybeActionRequirement.value()->isMet(this->skills, this->inventory, this->reserves);
}
