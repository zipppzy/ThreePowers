#include "world.h"

World::World(){
    currentId = 0;
    root = std::make_shared<Location>("root", 0, currentId, nullptr);
    currentId++;
}

void World::addLocation(std::string name, int position){
    std::shared_ptr<Location> newLocation = std::make_shared<Location>(name, position, currentId, root);
    locationList[currentId] = newLocation;
    root->addSubLocation(newLocation);
    currentId++;
}

void World::addLocation(std::string name,int position, std::shared_ptr<Location> parent){
    std::shared_ptr<Location> newLocation = std::make_shared<Location>(name, position, currentId, parent);
    locationList[currentId] = newLocation;
    parent->addSubLocation(newLocation);
    currentId++;
}

std::optional<std::shared_ptr<Location>> World::findLocation(int id){
    if(auto search = locationList.find(id); search !=locationList.end()){
        return search->second;
    }else{
        return std::nullopt;
    }
}
