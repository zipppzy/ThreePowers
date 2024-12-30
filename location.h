#ifndef LOCATION_H
#define LOCATION_H

#include <string>
#include <vector>
#include "action.h"

class Location
{
public:
    Location(std::string name);

    std::string name;
    std::vector<Action> actions;
    std::vector<Location> subLocations;
};

#endif // LOCATION_H
