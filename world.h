#ifndef WORLD_H
#define WORLD_H

#include "location.h"

class World
{
public:
    World();
    void addLocation(std::string name);
    void addLocation(std::string name, std::shared_ptr<Location> parent);
    std::optional<std::shared_ptr<const Location>> findLocation(int id);

private:
    std::shared_ptr<Location> root;
    unsigned int currentId;
    std::unordered_map<int, std::shared_ptr<Location>> locationList;
};

#endif // WORLD_H
