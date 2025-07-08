#include "location.h"

Location::Location(std::string name, int position, int id, std::shared_ptr<Location> parentLocation){
    this->name = name;
    this->position = position;
    this->id = id;
    this->parentLocation = parentLocation;
}

void Location::addSubLocation(std::shared_ptr<Location> location){
    subLocations.push_back(std::move(location));
}

void Location::addAction(Action action){
    actions.push_back(std::move(action));
}

std::vector<Action> Location::getActions(){
    return this->actions;
}
