#include "world.h"

World::World(){
    currentId = 0;
    root = std::make_shared<Location>("root", 0, currentId, nullptr);
    currentId++;
}

std::unordered_map<std::string, Location> locationDatabase;

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
