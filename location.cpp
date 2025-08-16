#include "location.h"

Location::Location(std::string name, int position, int id, bool visibleToPlayer, std::shared_ptr<Location> parentLocation)
    : name{name},
    position{position},
    id{id},
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
                actions.push_back(std::make_shared<Action>(*maybeAction));
            }
        }
    }
}


Location::Location(const Location& other)
    : name{other.name},
    position{other.position},
    id{other.id},
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

std::vector<std::shared_ptr<Action>> Location::getActions(){
    return this->actions;
}
