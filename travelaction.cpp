#include "travelaction.h"

TravelAction::TravelAction(std::string name, std::shared_ptr<Location> destination, std::shared_ptr<Location> currentLocation):
    //setting baseDuration to be = distance between locations
    //TODO: rework later
    Action(name, std::abs(currentLocation->getPosition() - destination->getPosition())),
    destination(destination){}

std::shared_ptr<Location> TravelAction::getDestination(){
    return this->destination;
}

double TravelAction::distanceToDestination(std::shared_ptr<Location> currentLocation){
    return std::abs(currentLocation->getPosition() - this->destination->getPosition());
}
