#ifndef RESERVE_H
#define RESERVE_H

#include "toml.hpp"
#include <string>
#include <vector>
// Represents a internal resource of the player
class Reserve
{
public:
    Reserve(std::string name, double maxValue);
    Reserve(toml::table reserveTable, Reserve* parentReserve);
    //Reserve(const Reserve& other);        //copy constructor

    double getMaxValue() const{return maxValue;}
    void setMaxValue(double value){this->maxValue = value;}
    double getCurrentValue() const{return currentValue;}
    bool add(double value);     //returns false if adding went over max
    bool addSub(std::string name, double value);
    bool subtract(double value);    //returns false if subtracting went below 0
    bool subtractSub(std::string name, double value);
    void addRegen(double value);
    void applyRegen();
    double getRegen() const;
    std::string name;

private:
    Reserve* parentReserve = nullptr;
    std::vector<Reserve> subReserves;   //if has subReserves main reserve should be sum of subReserves
    double maxValue;
    double currentValue = 0;
    double regen = 0;
    bool hasSubReserves = false;
    bool hasSoftMax = false;
};

#endif // RESERVE_H
