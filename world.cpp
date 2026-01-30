#include "world.h"

World::World(){
    root = std::make_shared<Location>("root", 0, currentIndex, false, nullptr);
    locationMap[0] = root;
    currentIndex = 1;
}

std::unordered_map<std::string, Location> World::locationDatabase;

void World::loadLocationDatabase(std::string path){
    auto in = toml::parse_file(path);

    World::locationDatabase.clear();
    if(auto locationsArray = in["locations"].as_array()){
        for(auto& elem : *locationsArray){
            if(auto locationTable = elem.as_table()){
                World::locationDatabase.insert_or_assign(locationTable->at("name").value_or(""), Location(*locationTable));
            }
        }
    }
}

std::optional<std::reference_wrapper<const Location>> World::checkLocationDatabase(std::string name){
    if(auto search = World::locationDatabase.find(name); search != World::locationDatabase.end()){
        return std::cref(search->second);
    }else{
        return std::nullopt;
    }
}

void World::loadWorldState(std::string path){
    auto in = toml::parse_file(path);

    this->locationMap.clear();
    this->lockedLocations.clear();
    this->hiddenLocations.clear();

    if(auto locationsArray = in["locations"].as_array()){
        for(auto&& elem : *locationsArray){
            if(auto locationTable = elem.as_table()){
                this->addLocation(locationTable->at("name").value_or(""), locationTable->at("position").value_or(0));
            }
        }
    }
}

void World::saveWorldState(std::string path){
    //TODO
}

void World::addLocation(std::string name, int position){
    addLocation(std::move(name), position, this->root);
}

void World::addLocation(std::string name,int position, std::shared_ptr<Location> parent){
    std::shared_ptr<Location> newLocation;
    if(auto maybeLocation = World::checkLocationDatabase(name)){
        //found template in locationDatabase
        newLocation = std::make_shared<Location>(*maybeLocation);
        newLocation->setIndex(currentIndex);    //set here because template does not have index
        newLocation->setParent(parent);
        newLocation->setPosition(position);

        // Apply default hidden/locked states from template
        if (newLocation->defaultHidden) {
            hiddenLocations.insert(name);
        }
        if (newLocation->defaultLocked) {
            lockedLocations.insert(name);
        }

        //recursively creates subLocations
        for(std::string& subLocationName : newLocation->subLocationNames){
            this->addLocation(subLocationName, newLocation->getPosition(), newLocation);
        }
        //create travel actions to parent and children
        if(newLocation->getParent() != nullptr && newLocation->getParent()->name != "root"){
            newLocation->addAction(std::make_shared<TravelAction>("Travel To " + newLocation->getParent()->name, newLocation->getParent(), newLocation));
        }
        for(auto& childLocation : newLocation->getSubLocations()){
            newLocation->addAction(std::make_shared<TravelAction>("Travel To " + childLocation->name, childLocation, newLocation));
        }
    }else{
        //default case if no template is found
        newLocation = std::make_shared<Location>(name, position, currentIndex, true, root);
    }

    locationMap[currentIndex] = newLocation;
    parent->addSubLocation(newLocation);
    currentIndex++;
}

std::optional<std::shared_ptr<Location>> World::findLocation(int id) const{
    if(auto search = locationMap.find(id); search !=locationMap.end()){
        return search->second;
    }else{
        return std::nullopt;
    }
}

std::optional<std::shared_ptr<Location>> World::findLocation(const std::string& name) const{
    for(const auto& [id, location] : locationMap){
        if(location && location->name == name){
            return location;
        }
    }
    return std::nullopt;
}

void World::unlockLocation(const std::string& locationName) {
    if (lockedLocations.erase(locationName)) {
        Logger::log("Unlocked location: " + locationName);
        // Unlocking also makes it visible
        hiddenLocations.erase(locationName);
    }
}

void World::lockLocation(const std::string& locationName) {
    if (lockedLocations.insert(locationName).second) {
        Logger::log("Locked location: " + locationName);
    }
}

void World::unhideLocation(const std::string& locationName) {
    if (hiddenLocations.erase(locationName)) {
        Logger::log("Discovered location: " + locationName);
    }
}

void World::hideLocation(const std::string& locationName) {
    if (hiddenLocations.insert(locationName).second) {
        Logger::log("Hidden location: " + locationName);
    }
}

bool World::isLocationUnlocked(const std::string& locationName) const {
    return lockedLocations.count(locationName) == 0;
}

bool World::isLocationVisible(const std::string& locationName) const {
    return hiddenLocations.count(locationName) == 0;
}

const std::set<std::string>& World::getLockedLocations() const {
    return lockedLocations;
}

const std::set<std::string>& World::getHiddenLocations() const {
    return hiddenLocations;
}
