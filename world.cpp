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

std::optional<std::shared_ptr<Location>> World::findLocation(int id){
    if(auto search = locationMap.find(id); search !=locationMap.end()){
        return search->second;
    }else{
        return std::nullopt;
    }
}
