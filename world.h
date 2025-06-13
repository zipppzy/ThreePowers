#ifndef WORLD_H
#define WORLD_H

#include "location.h"

class World
{
public:
    World();
    void addLocation(std::string name, int position);
    void addLocation(std::string name, int position, std::shared_ptr<Location> parent);
    std::optional<std::shared_ptr<const Location>> findLocation(int id);

private:
    std::shared_ptr<Location> root;
    unsigned int currentId;
    //perhaps should be unique pointer instead of shared ptr
    std::unordered_map<int, std::shared_ptr<Location>> locationList;
    //TODO: locationDatabase at some point
};

#endif // WORLD_H
