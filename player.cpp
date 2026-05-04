#include "player.h"


Player::Player() {
}

Player::Player(unsigned long long int age, unsigned long long int lifespan, std::shared_ptr<Location> currentLocation, World* world)
    : age{age},
    lifespan{lifespan},
    currentLocation{currentLocation},
    maxWeight{50},
    currentWeight{0},
    triggerContext{&this->skills, &this->reserves, &this->inventory, &this->visitedLocations},
    world{world}
{
    this->triggerManager.loadTriggers("config/triggers.toml");
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
    this->applySkillEffectsAllActions();
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
            addItemActions(item);
        }
        this->currentWeight = item.getWeight()*item.count;
        inventoryChanged = true;
        return true;
    }
}

bool Player::removeItem(const std::string& itemName, int count){
    auto it = std::find_if(inventory.begin(), inventory.end(),
                           [&itemName](const Item& i){ return i.name == itemName; });
    if(it == inventory.end()) return false;
    if(static_cast<int>(it->count) < count) return false;

    this->currentWeight -= it->getWeight() / it->count * count;  // weight per unit * removed count
    it->count -= count;

    if(it->count == 0){
        removeItemActions(itemName);
        inventory.erase(it);
    }

    inventoryChanged = true;
    return true;
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

const std::vector<Item>& Player::getInventory() const{
    return inventory;
}

const std::unordered_map<std::string, std::vector<std::shared_ptr<Action>>>& Player::getItemActions() const{
    return itemActions;
}

std::shared_ptr<Location> Player::getCurrentLocation(){
    return currentLocation;
}

void Player::applySkillEffectsToAction(std::shared_ptr<Action> action){
    std::vector<std::pair<Effect, int>> skillEffectList;

    for(const auto& skill : skills){
        if(auto maybeEffects = skill.checkEffects(action->name)){
            for(const auto& effect : maybeEffects->get()){
                skillEffectList.emplace_back(effect, skill.getLevel());
            }
        }
    }

    // applyEffects resets to base values first so always safe to call
    action->applyEffects(skillEffectList);
}

void Player::applySkillEffectsAllActions(){
    if(skills.empty()) return;

    for(auto& action : currentLocation->getActions()){
        applySkillEffectsToAction(action);
    }

    for(auto& action : globalActions){
        applySkillEffectsToAction(action);
    }

    for(auto& [itemName, actions] : itemActions){
        for(auto& action : actions){
            applySkillEffectsToAction(action);
        }
    }
}

void Player::moveLocation(std::shared_ptr<Location> destination){
    this->currentLocation = destination;
    this->movedLocation = true;
    this->visitedLocations.insert(destination->name);
    this->triggerManager.onLocationChange(this->triggerContext, destination->name);
    this->applySkillEffectsAllActions();
}

void Player::unlockLocation(const std::string& locationName) {
    if (world){
        world->unlockLocation(locationName);
        this->movedLocation = true;  // Trigger UI update
    }
}

void Player::lockLocation(const std::string& locationName) {
    if (world){
        world->lockLocation(locationName);
        this->movedLocation = true;
    }
}

void Player::unhideLocation(const std::string& locationName) {
    if (world){
        world->unhideLocation(locationName);
        this->movedLocation = true;
    }
}

void Player::hideLocation(const std::string& locationName) {
    if (world) {
        world->hideLocation(locationName);
        this->movedLocation = true;
    }
}

void Player::setRestAction(std::shared_ptr<Action> action){
    this->restAction = action;
}

void Player::applyInstantEffect(const InstantEffect& effect) {
    switch (effect.type) {
        case InstantEffect::Log: {
            std::string message = effect.parameters.count("message")
            ? effect.parameters.at("message") : "";
            Logger::log(message);
            break;
        }

        case InstantEffect::Event: {
            std::string eventId = effect.parameters.count("eventId")
            ? effect.parameters.at("eventId") : "";
            queueEvent(eventId);
            break;
        }

        case InstantEffect::UnlockAction: {
            std::string actionName = effect.parameters.count("actionName")
            ? effect.parameters.at("actionName") : "";
            if (auto maybeAction = Action::checkActionDatabaseDatabase(actionName)) {
                addGlobalAction(std::make_shared<Action>(maybeAction.value()));
                Logger::log("Unlocked new action: " + actionName);
            }
            break;
        }

        case InstantEffect::AddSkillXp: {
            std::string skillName = effect.parameters.count("skillName") ? effect.parameters.at("skillName") : "";
            double xp = effect.parameters.count("xp") ? std::stod(effect.parameters.at("xp")) : 0.0;
            addSkillXp(skillName, xp);
            break;
        }

        case InstantEffect::ModifyReserve: {
            std::string reserveName = effect.parameters.count("reserveName")
            ? effect.parameters.at("reserveName") : "";
            double amount = effect.parameters.count("amount")
                                ? std::stod(effect.parameters.at("amount")) : 0.0;
            addReserve(reserveName, amount);
            break;
        }

        case InstantEffect::AddItem: {
            std::string itemName = effect.parameters.count("itemName") ? effect.parameters.at("itemName") : "";
            int count = effect.parameters.count("count") ? std::stoi(effect.parameters.at("count")) : 1;

            if(auto maybeItem = Item::checkItemDatabase(itemName)){
                Item item = maybeItem->get();
                item.count = count;
                this->pickupItem(item);
                Logger::log("Received " + std::to_string(count) + "x " + itemName);
            }
            break;
        }

        case InstantEffect::UnlockLocation: {
            std::string locationName = effect.parameters.count("locationName") ? effect.parameters.at("locationName") : "";
            this->unlockLocation(locationName);
            break;
        }

        case InstantEffect::LockLocation: {
            std::string locationName = effect.parameters.count("locationName") ? effect.parameters.at("locationName") : "";
            lockLocation(locationName);
            break;
        }

        case InstantEffect::UnhideLocation: {
            std::string locationName = effect.parameters.count("locationName") ? effect.parameters.at("locationName") : "";
            unhideLocation(locationName);
            break;
        }

        case InstantEffect::HideLocation: {
            std::string locationName = effect.parameters.count("locationName") ? effect.parameters.at("locationName") : "";
            hideLocation(locationName);
            break;
        }

        case InstantEffect::UnlockResearchTopic: {
            std::string topicName = effect.parameters.count("topicName") ? effect.parameters.at("topicName") : "";
            unlockResearchTopic(topicName);
            break;
        }

        case InstantEffect::AddResearchNote: {
            if(this->activeResearchTopic.empty()){
                Logger::log("No active research topic selected");
                break;
            }

            auto maybeMemory = findReserve("Memory");
            if (!maybeMemory) break;
            Reserve* memory = maybeMemory.value();

            double memoryConsumed = memory->getCurrentValue();
            if (memoryConsumed <= 0.0) {
                Logger::log("Nothing in memory");
                break;
            }

            addResearchNote(ResearchNote(memoryConsumed, 0, this->activeResearchTopic));
            break;
        }

    }
}

void Player::queueEvent(const std::string& eventId){
    pendingEvents.push_back(eventId);
}

void Player::addResearchNote(const ResearchNote& note) {
    auto it = researchTopics.find(note.topicName);
    if (it == researchTopics.end()) return; // topic not unlocked yet
    it->second.addNote(note);
    researchChanged = true;
}

bool Player::mergeNotes(const std::string& topicName, int tier, double focusSpent) {
    auto it = researchTopics.find(topicName);
    if (it == researchTopics.end()) return false;

    // Focus cost scales with tier - higher tiers cost dramatically more
    // Tier 0->1: base cost; Tier 1->2: 4x; Tier 2->3: 16x etc.
    double maxFocusCost = 20.0 * std::pow(4.0, tier);

    if (auto resultNote = it->second.merge(tier, focusSpent, maxFocusCost)) {
        // Deduct focus
        if (auto focus = findReserve("Focus")) {
            focus.value()->subtract(std::min(focusSpent, focus.value()->getCurrentValue()));
        }
        it->second.addNote(*resultNote);
        researchChanged = true;
        Logger::log("Merged " + ResearchNote::tierName(tier) + "s into a "
                    + ResearchNote::tierName(tier + 1)
                    + " on topic: " + topicName);
        return true;
    }
    return false;
}

bool Player::attemptResearch(const std::string& topicName) {
    auto it = researchTopics.find(topicName);
    if (it == researchTopics.end()) return false;

    ResearchTopic& topic = it->second;

    if (topic.successChance() <= 0.0) {
        Logger::log("Not enough knowledge to attempt: " + topicName);
        return false;
    }

    // Roll for success
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double roll = dist(gen);

    bool success = topic.attempt(roll);
    researchChanged = true;

    if (success) {
        Logger::log("Research breakthrough: " + topicName + "!");
        for (const auto& effect : topic.completionEffects) {
            applyInstantEffect(effect);
        }
    } else {
        Logger::log("Research attempt failed for: " + topicName
                    + ". Progress lost but insight gained.");
    }

    return success;
}

bool Player::hasResearchTopic(const std::string& topicName) const {
    return researchTopics.count(topicName) > 0;
}

void Player::unlockResearchTopic(const std::string& topicName) {
    if (this->researchTopics.count(topicName)) return; // already unlocked

    if (auto maybeTemplate = ResearchTopic::checkResearchDatabase(topicName)) {
        this->researchTopics.insert_or_assign(topicName, maybeTemplate->get());
        Logger::log("New research topic available: " + topicName);
        this->researchChanged = true;
    }
}

const std::unordered_map<std::string, ResearchTopic>& Player::getResearchTopics() const {
    return researchTopics;
}

void Player::handleAutoRest(Reserve* targetReserve){
    //check if rest has const gain to the reserve that ran out
    if(this->restAction->getConstantReserveGain().find(targetReserve->name) != this->restAction->getConstantReserveGain().end()){
        this->actionQueue.push_front({this->restAction, -1});
        this->isAutoResting = true;
        this->targetReserve = targetReserve;
        this->removeConstActionReserveModifiers(currentAction);
    }else{
        this->endCurrentAction();
    }
    this->attemptStartNextAction();
}

const std::shared_ptr<Action> Player::getRestAction() const{
    return this->restAction;
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

unsigned long long int Player::fastForwardQueue(){
    unsigned long long int ticksSimulated = 0;
    while(!actionQueue.empty()){
        this->tick();
        ticksSimulated++;
    }
    return ticksSimulated;
}

void Player::consumeResources(const ConsumeList& toConsume) {
    // Consume items
    for (const auto& itemPair : toConsume.items) {
        removeItem(itemPair.first, itemPair.second);
    }

    // Consume reserves
    for (const auto& reservePair : toConsume.reserves) {
        const std::string& reserveName = reservePair.first;
        double amount = reservePair.second;

        if (auto reserve = this->findReserve(reserveName)) {
            reserve.value()->subtract(amount);
        }
    }
}

bool Player::startAction(std::shared_ptr<Action> action){
    if(checkActionRequirements(action)){
        // Consume items/reserves before starting
        if(auto consReq = action->getConsumablesRequirement()) {
            ConsumeList toConsume = consReq->getConsumeList(this->inventory, this->reserves);
            consumeResources(toConsume);
        }

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
                    //check if autoRest is on
                    if(this->autoRest && this->restAction != nullptr){
                        this->handleAutoRest(maybeReserve.value());
                    }else{
                        this->endCurrentAction();
                    }
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
                for(const InstantEffect& instantEffect : currentAction->getCompletionEffects()){
                    this->applyInstantEffect(instantEffect);
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
        //handle ending of autoRest
        if(this->isAutoResting){
            if(this->targetReserve->getCurrentValue() >= this->targetReserve->getMaxValue()){
                this->endCurrentAction();
                this->attemptStartNextAction();
                this->isAutoResting = false;
            }
        }
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
    // Check conditions (if they exist)
    if (auto condReq = action->getConditionsRequirement()) {
        if (!condReq->isMet(this->skills, this->inventory, this->reserves)) {
            return false;
        }
    }

    // Check consumables (if they exist)
    if (auto consReq = action->getConsumablesRequirement()) {
        if (!consReq->isMet(this->skills, this->inventory, this->reserves)) {
            return false;
        }
    }

    return true;
}

void Player::addItemActions(const Item& item){
    if(item.actions.empty()) return;

    // Already registered (e.g. picking up a second copy of the same item)
    if(itemActions.count(item.name)) return;

    std::vector<std::shared_ptr<Action>> resolved;
    for(const std::string& actionName : item.actions){
        if(auto maybeAction = Action::checkActionDatabaseDatabase(actionName)){
            resolved.push_back(std::make_shared<Action>(maybeAction->get()));
        }else{
            qDebug() << "Item" << QString::fromStdString(item.name) << "references unknown action:" << QString::fromStdString(actionName);
        }
    }

    if(!resolved.empty()){
        itemActions[item.name] = std::move(resolved);
        itemActionsChanged = true;
    }
}

void Player::removeItemActions(const std::string& itemName){
    if(itemActions.erase(itemName)){
        itemActionsChanged = true;
    }
}
