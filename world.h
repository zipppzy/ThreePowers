#ifndef WORLD_H
#define WORLD_H

#include "location.h"

class World
{
public:
    World();

    static void loadLocationDatabase(std::string path);
    static std::optional<std::reference_wrapper<const Location>> checkLocationDatabase(std::string name);

    void addLocation(std::string name, int position);
    void addLocation(std::string name, int position, std::shared_ptr<Location> parent);
    std::optional<std::shared_ptr<Location>> findLocation(int id);

private:
    static std::unordered_map<std::string, Location> locationDatabase;
    std::shared_ptr<Location> root;
    unsigned int currentId;
    //perhaps should be unique pointer instead of shared ptr
    std::unordered_map<int, std::shared_ptr<Location>> locationList;

};

#endif // WORLD_H
