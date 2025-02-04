#include "player.h"


Player::Player() {
}

Player::Player(unsigned long long int age, unsigned long long int lifespan, Location* currentLocation, Action* currentAction){
    this->age = age;
    this->lifespan = lifespan;
    this->currentLocation = currentLocation;
    this->currentAction = currentAction;
    this->qi = 0;
    this->maxQi = 100;
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

void Player::addSkillXp(std::string name, double xp){
    if(this->hasSkill(name)){
        for(Skill i : this->skills){
            if(i.name == name){
                i.addXp(xp);
            }
        }
    }else{
        auto result = Skill::checkSkillDatabase(name);
        if(result){
            this->skills.push_back(Skill(result->get()));
        }else{
            qDebug() << "Tried to add xp to non-valid skill";
        }
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

void Player::savePlayerState(){
    toml::table playerData;

    toml::table attributes;
    attributes.insert("age", std::to_string(age));              //stored as string to avoid length limit
    attributes.insert("lifespan", std::to_string(lifespan));    //stored as string to avoid length limit
    attributes.insert("qi", qi);
    attributes.insert("maxQi", maxQi);

    playerData.insert("attributes", std::move(attributes));

    toml::array skills;
    for(Skill i : this->skills){
        skills.push_back(i.getSkillTable());
    }

    playerData.insert("skills", std::move(skills));

    std::ofstream file("player_state.toml");
    file << playerData;
    file.close();
}

void Player::loadPlayerState(){
    auto playerData = toml::parse_file("player_state.toml");

    auto attributes = playerData["attributes"];
    this->age = std::stoull(playerData["age"].value_or("0"));
    this->lifespan = std::stoull(playerData["lifespan"].value_or("0"));
    this->qi = attributes["qi"].value_or(0.0);
    this->maxQi = attributes["maxQi"].value_or(0.0);

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
    //ticks currentAction and check if action is completed then gives rewards
    if(currentAction->tick()){
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
    }
}

bool Player::checkActionRequirements(Action* action) const{
    return action->getActionRequirements().get()->isMet(this->skills, this->inventory);
}
