#include "world.h"

World::World(){
    currentId = 0;
}

void World::addLocation(std::string name){
    std::shared_ptr<Location> newLocation = std::make_shared<Location>(name, currentId, root);
    locationList[currentId] = newLocation;
    root->addSubLocation(newLocation);
    currentId++;
}

void World::addLocation(std::string name, std::shared_ptr<Location> parent){
    std::shared_ptr<Location> newLocation = std::make_shared<Location>(name, currentId, parent);
    locationList[currentId] = newLocation;
    parent->addSubLocation(newLocation);
    currentId++;
}

std::optional<std::shared_ptr<const Location>> World::findLocation(int id){
    if(auto search = locationList.find(id); search !=locationList.end()){
        return search->second;
    }else{
        return std::nullopt;
    }
}
