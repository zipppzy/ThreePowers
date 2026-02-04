#include "item.h"


Item::Item()
    : name(""),
    size(0.0),
    weight(0.0),
    count(0)
{}

Item::Item(std::string name, double size, double weight, unsigned int count){
    this->name = name;
    this->size = size;
    this->weight = weight;
    this->count = count;
}

Item::Item(toml::table itemTable){
    this->name = itemTable["name"].value_or("");
    this->size = itemTable["size"].value<double>().value_or(0.0);
    this->weight = itemTable["weight"].value<double>().value_or(0.0);
    this->count = itemTable["count"].value<int64_t>().value_or(0);
}

double Item::getWeight() const{
    return this->weight * this->count;
}

double Item::getSize() const{
    return this->size * this->count;
}

std::unordered_map<std::string, Item> Item::itemDatabase;

void Item::loadItemDatabase(std::string path){
    auto in = toml::parse_file(path);

    Item::itemDatabase.clear();
    if(auto itemsArray = in["items"].as_array()){
        for(auto& elem : *itemsArray){
            if(auto itemTable = elem.as_table()){
                // Create item with count 0 (template)
                Item templateItem(*itemTable);
                templateItem.count = 0; // Database stores templates with no count
                Item::itemDatabase[itemTable->at("name").value_or("")] = templateItem;
            }
        }
    }
}

std::optional<std::reference_wrapper<const Item>> Item::checkItemDatabase(std::string name){
    if(auto search = Item::itemDatabase.find(name); search != Item::itemDatabase.end()){
        return std::cref(search->second);
    }else{
        return std::nullopt;
    }
}
