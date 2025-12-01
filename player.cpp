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
auto Player::findSkillIter(std::string name){
    return std::find_if(this->skills.begin(), this->skills.end(), [&name](const Skill& skill){return skill.name == name;});
}

const std::vector<Skill>& Player::getSkills() const{
    return skills;
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
    auto it = this->findSkillIter(name);
    if(it != skills.end()){
        it -> addXp(xp);
        updatedSkillIndexes.push_back(std::distance(skills.begin(), it));
    }else{
        auto result = Skill::checkSkillDatabase(name);
        if(result){
            this->skills.push_back(Skill(result->get()));
            skills.back().addXp(xp);
            newSkillIndexes.push_back(skills.size()-1);
        }else{
            qDebug() << "Tried to add xp to non-valid skill";
        }
    }
    this->applySkillEffectsCurrentLocation();
}

void Player::addNewReserve(const Reserve& reserve){
    this->reserves.push_back(Reserve(reserve));
}

void Player::addReserve(const std::string& name, double gain){
    if(auto existingReserve = this->findReserve(name)){
        existingReserve.value()->add(gain);
    }
}

const std::vector<Reserve>& Player::getReserves() const{
    return reserves;
}

void Player::applyActionConstReserveModifiers(std::shared_ptr<Action> action){
    auto constantReserveGain = action->getConstantReserveGain();
    for(const auto& [reserveName, gain] : constantReserveGain){
        if(auto reserve = this->findReserve(reserveName)){
            reserve.value()->addRegen(gain);
        }
    }

    auto constantReserveCost = action->getConstantReserveCost();
    for(const auto& [reserveName, cost] : constantReserveCost){
        if(auto reserve = this->findReserve(reserveName)){
            reserve.value()->addRegen(-cost);
        }
    }
}

void Player::removeConstActionReserveModifiers(std::shared_ptr<Action> action){
    auto constantReserveGain = action->getConstantReserveGain();
    for(const auto& [reserveName, gain] : constantReserveGain){
        if(auto reserve = this->findReserve(reserveName)){
            reserve.value()->addRegen(-gain);
        }
    }

    auto constantReserveCost = action->getConstantReserveCost();
    for(const auto& [reserveName, cost] : constantReserveCost){
        if(auto reserve = this->findReserve(reserveName)){
            reserve.value()->addRegen(cost);
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
        if(!skills.empty()){
            action->applyEffects(skillEffectList);
        }
    }
}

void Player::moveLocation(std::shared_ptr<Location> destination){
    this->currentLocation = destination;
    this->movedLocation = true;
}

void Player::addGlobalAction(std::shared_ptr<Action> action){
    this->globalActions.push_back(action);
}

const std::vector<std::shared_ptr<Action>>& Player::getGlobalActions() const{
    return this->globalActions;
}

const std::deque<std::pair<std::shared_ptr<Action>, int>>& Player::getActionQueue() const{
    return actionQueue;
}

void Player::addActionToQueue(std::shared_ptr<Action> action, int numRepeats){
    if(!actionQueue.empty() && action->name == actionQueue.back().first->name){
        this->actionQueue.back().second += numRepeats;
    }else{
        this->actionQueue.push_back({action, numRepeats});
    }
}

void Player::removeActionFromQueue(int index, int numRemoved){
    // If removing the currently executing action (index 0), remove its reserve modifiers
    if(index == 0 && currentAction != nullptr){
        removeConstActionReserveModifiers(currentAction);
        currentAction = nullptr;
    }

    this->actionQueue.at(index).second -= numRemoved;
    if(actionQueue.at(index).second <= 0){
        this->actionQueue.erase(actionQueue.begin()+index);
    }
    attemptStartNextAction();
}

void Player::moveUpActionInQueue(int index){
    if(index <= 0) return;
    std::swap(actionQueue[index], actionQueue[index-1]);
}

void Player::endCurrentAction(){
    removeConstActionReserveModifiers(currentAction);
    actionQueue.pop_front();
    currentAction = nullptr;
}

void Player::attemptStartNextAction(){
    if(!this->actionQueue.empty()){
        if(!startAction(this->actionQueue.front().first)){
            actionQueue.pop_front();
            attemptStartNextAction();
        }
    }
}

void Player::fastForwardQueue(){
    while(!actionQueue.empty()){
        this->tick();
    }
}

bool Player::startAction(std::shared_ptr<Action> action){
    if(checkActionRequirements(action)){
        this->currentAction = action;
        this->applyActionConstReserveModifiers(action);
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
        // Check if any reserve with a cost has run out
        auto constantReserveCost = currentAction->getConstantReserveCost();
        for(const auto& [reserveName, cost] : constantReserveCost){
            if(auto maybeReserve = this->findReserve(reserveName)){
                if(maybeReserve.value()->getCurrentValue() <= 0){
                    // Remove reserve modifiers before ending action
                    endCurrentAction();
                    attemptStartNextAction();
                    return;
                }
            }
        }
        //apply reserve regen
        for(Reserve& reserve : this->reserves){
            reserve.applyRegen();
        }
        //ticks currentAction and check if action is completed then gives rewards
        if(currentAction->tick()){
            if(currentAction->isSuccess()){
                if(auto travelAction = std::dynamic_pointer_cast<TravelAction>(currentAction)){
                    this->moveLocation(travelAction->getDestination());
                    this->movedLocation = true;
                }
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
                for(const auto& [reserveName,gain] : reserveRewards){
                    this->addReserve(reserveName, gain);
                }
            }else{
                qDebug("Action Failed");
            }

            //if current action is not repeating
            if(actionQueue.front().second != -1){
                //if we have completed all repeats of current action pop and move to next in queue
                actionQueue.front().second--;
                if(actionQueue.front().second == 0){
                    endCurrentAction();
                    attemptStartNextAction();
                    return;
                }
            }
        }

        // //handle constant rewards and costs
        // auto constantReserveGain = currentAction->getConstantReserveGain();
        // if(!constantReserveGain.empty()){
        //     for(const auto& [reserveName,gain] : constantReserveGain){
        //         this->addReserve(reserveName, gain);
        //     }
        // }

        // auto constantReserveCost = currentAction->getConstantReserveCost();
        // for(const auto& [reserveName,cost] : constantReserveCost){
        //     this->addReserve(reserveName, -cost);
        //     //if reserve runs out move to next action
        //     if(auto maybeReserve = this->findReserve(reserveName)){
        //         if(maybeReserve.value()->getCurrentValue() <= 0){
        //             actionQueue.pop_front();
        //             currentAction = nullptr;
        //             attemptStartNextAction();
        //         }
        //     }
        // }
    }else{
        attemptStartNextAction();
    }

}

void Player::tick(int numTicks){
    for(int i=0; i<numTicks; i++){
        this->tick();
    }
}

bool Player::checkActionRequirements(std::shared_ptr<Action> action) const{
    auto maybeActionRequirement = action->getActionRequirements();
    if(!maybeActionRequirement){
        return true;
    }
    return maybeActionRequirement.value()->isMet(this->skills, this->inventory, this->reserves);
}
