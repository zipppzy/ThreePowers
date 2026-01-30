#ifndef LOCATION_H
#define LOCATION_H

#include <string>
#include <vector>
#include "action.h"

class Location{
public:
    Location(std::string name, int position, int index, bool visibleToPlayer = false, std::shared_ptr<Location> parentLocation = nullptr);
    Location(toml::table locationTable);
    Location(const Location& other);

    std::string name;
    bool defaultHidden = false;
    bool defaultLocked = false;

    std::vector<std::string> subLocationNames;

    void addSubLocation(std::shared_ptr<Location> location);
    void addAction(std::shared_ptr<Action> action);
    void setIndex(int index);
    void setParent(std::shared_ptr<Location> parent);
    void setPosition(int pos);

    std::shared_ptr<Location> getParent() const;
    std::vector<std::shared_ptr<Location>> getSubLocations() const;
    std::vector<std::shared_ptr<Action>> getActions() const;
    int getPosition() const;

private:
    int position;   //location on a 1d line; 0 is starting location; - is left + is right;
    int index;         //corresponds to index in world.locationList
    std::vector<std::shared_ptr<Action>> actions;    //locations own the actions
    std::vector<std::shared_ptr<Location>> subLocations;
    std::shared_ptr<Location> parentLocation;
};

#endif // LOCATION_H
