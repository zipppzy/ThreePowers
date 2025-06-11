#include "location.h"

Location::Location(std::string name, std::shared_ptr<Location> parentLocation){
    this->name = name;
    this->id = Location::nextId;
    Location::nextId++;
    this->parentLocation = parentLocation;
}

void Location::addSubLocation(std::shared_ptr<Location> location){
    subLocations.push_back(std::move(location));
}

void Location::addAction(std::shared_ptr<Action> action){
    actions.push_back(std::move(action));
}
