#include "item.h"

Item::Item(std::string name, double size, double weight, unsigned int count){
    this->name = name;
    this->size = size;
    this->weight = weight;
    this->count = count;
}

Item::Item(toml::table itemTable){
    this->name = itemTable["name"].value_or("");
    this->size = itemTable["size"].value_or(-1);
    this->weight = itemTable["weight"].value_or(-1);
    this->count = itemTable["count"].value_or(-1);
}

double Item::getWeight() const{
    return this->weight;
}

double Item::getSize() const{
    return this->size;
}
