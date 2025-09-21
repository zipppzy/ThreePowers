#include "location.h"

Location::Location(std::string name, int position, int index, bool visibleToPlayer, std::shared_ptr<Location> parentLocation)
    : name{name},
    position{position},
    index{index},
    visibleToPlayer{visibleToPlayer},
    parentLocation{parentLocation}
{}

//id, position, parent and child locations should be set by world
Location::Location(toml::table locationTable){
    this->name = locationTable["name"].value_or("");
    this->visibleToPlayer = locationTable["visibleToPlayer"].value_or(false);

    if(auto actionsArray = locationTable["actions"].as_array()){
        for(auto&& actionName : *actionsArray){
            if(auto maybeAction = Action::checkActionDatabaseDatabase(actionName.value_or(""))){
                this->actions.push_back(std::make_shared<Action>(*maybeAction));
            }
        }
    }

    if(auto subLocationNamesArray = locationTable["subLocationNames"].as_array()){
        this->subLocationNames.clear();
        for(auto&& subLocationName : *subLocationNamesArray){
            subLocationNames.push_back(subLocationName.value_or(""));
        }
    }
}


Location::Location(const Location& other)
    : name{other.name},
    position{other.position},
    index{other.index},
    visibleToPlayer{other.visibleToPlayer}
{
    for (const auto& action : other.actions) {
        if (action) {
            actions.push_back(std::make_shared<Action>(*action));
        }
    }

    for (const auto& subLoc : other.subLocations) {
        if (subLoc) {
            subLocations.push_back(std::make_shared<Location>(*subLoc));
        }
    }
}

void Location::addSubLocation(std::shared_ptr<Location> location){
    subLocations.push_back(std::move(location));
}

void Location::addAction(Action action){
    actions.push_back(std::make_shared<Action>(action));
}

void Location::setIndex(int index){
    this->index = index;
}

std::vector<std::shared_ptr<Action>> Location::getActions(){
    return this->actions;
}

int Location::getPosition(){
    return this->position;
}
