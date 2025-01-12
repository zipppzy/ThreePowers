#ifndef LOCATION_H
#define LOCATION_H

#include <string>
#include <vector>
#include "action.h"
#include "toml.hpp"

class Location
{
public:
    Location(std::string name);

    std::string name;

private:
    std::vector<Action> actions;
    std::vector<Location> subLocations;
    Location* parentLocation;
};

#endif // LOCATION_H
