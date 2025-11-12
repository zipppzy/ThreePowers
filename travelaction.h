#ifndef TRAVELACTION_H
#define TRAVELACTION_H

#include "action.h"
#include "location.h"

class TravelAction : public Action
{
public:
    TravelAction(std::string name, std::shared_ptr<Location> destination, std::shared_ptr<Location> currentLocation);

    std::shared_ptr<Location> getDestination();

private:
    double distanceToDestination(std::shared_ptr<Location> currentLocation);

    std::shared_ptr<Location> destination = nullptr;
};

#endif // TRAVELACTION_H
