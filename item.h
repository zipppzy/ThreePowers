#ifndef ITEM_H
#define ITEM_H

#include "toml.hpp"
#include <string>
#include <optional>
#include <functional>
#include <unordered_map>

class Item
{
public:
    Item();
    Item(std::string name, double size, double weight, unsigned int count);
    Item(toml::table itemTable);

    double getWeight() const;
    double getSize() const;
    unsigned int count;          //number of items in the stack

    std::string name;

    static void loadItemDatabase(std::string path);
    static std::optional<std::reference_wrapper<const Item>> checkItemDatabase(std::string name);
private:
    static std::unordered_map<std::string, Item> itemDatabase;

    double size;        //individual item size in meters^3
    double weight;      //individual item weight in kg
};

#endif // ITEM_H
