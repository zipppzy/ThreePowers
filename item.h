#ifndef ITEM_H
#define ITEM_H

#include "toml.hpp"
#include <string>

class Item
{
public:
    Item(std::string name, double size, double weight, unsigned int count);
    Item(toml::table itemTable);

    double getWeight() const;
    double getSize() const;
    unsigned int count;          //number of items in the stack

    std::string name;

private:
    double size;        //individual item size in meters^3
    double weight;      //individual item weight in kg
};

#endif // ITEM_H
