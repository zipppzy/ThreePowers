#include "location.h"

Location::Location(std::string name, unsigned int id, std::shared_ptr<Location> parentLocation){
    this->name = name;
    this->id = id;
    this->parentLocation = parentLocation;
}

void Location::addSubLocation(std::shared_ptr<Location> location){
    subLocations.push_back(location);
}
