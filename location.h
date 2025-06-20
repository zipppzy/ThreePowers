#ifndef LOCATION_H
#define LOCATION_H

#include <string>
#include <vector>
#include "action.h"

class Location{
public:
    Location(std::string name, int position, int id, std::shared_ptr<Location> parentLocation);
    std::string name;
    bool visibleToPlayer;
    void addSubLocation(std::shared_ptr<Location> location);
    void addAction(std::shared_ptr<Action> action);
    std::vector<std::shared_ptr<Action>> getActions();

private:
    int position;   //location on a 1d line; 0 is starting location; - is left + is right;
    int id;         //corresponds to index in world.locationList
    std::vector<std::shared_ptr<Action>> actions;
    std::vector<std::shared_ptr<Location>> subLocations;
    std::shared_ptr<Location> parentLocation;
};

#endif // LOCATION_H
