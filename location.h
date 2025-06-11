#ifndef LOCATION_H
#define LOCATION_H

#include <string>
#include <vector>
#include "action.h"

class Location{
public:
    Location(std::string name, std::shared_ptr<Location> parentLocation);
    std::string name;
    bool visibleToPlayer;
    void addSubLocation(std::shared_ptr<Location> location);
    void addAction(std::shared_ptr<Action> action);

private:
    static int nextId;
    int position;   //location on a 1d line; 0 is starting location; - is left + is right;
    unsigned int id;    //should be unique for each location
    std::vector<std::shared_ptr<Action>> actions;
    std::vector<std::shared_ptr<Location>> subLocations;
    std::shared_ptr<Location> parentLocation;
};

int Location::nextId = 0;

#endif // LOCATION_H
