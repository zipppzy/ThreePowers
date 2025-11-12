#ifndef WORLD_H
#define WORLD_H

#include "location.h"
#include "travelaction.h"

class World
{
public:
    World();

    static void loadLocationDatabase(std::string path);
    static std::optional<std::reference_wrapper<const Location>> checkLocationDatabase(std::string name);

    void loadWorldState(std::string path);
    void saveWorldState(std::string path);  //TODO

    void addLocation(std::string name, int position);
    void addLocation(std::string name, int position, std::shared_ptr<Location> parent);
    std::optional<std::shared_ptr<Location>> findLocation(int id);

private:
    //locationDatabase contains templates to make locations
    //does not define position
    static std::unordered_map<std::string, Location> locationDatabase;
    std::shared_ptr<Location> root;
    unsigned int currentIndex;

    //perhaps should be unique pointer instead of shared ptr?
    //easy to reference map of existing locations
    std::unordered_map<int, std::shared_ptr<Location>> locationMap;

};

#endif // WORLD_H
