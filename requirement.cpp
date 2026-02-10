#include "requirement.h"

Requirement::Requirement(const toml::table& node){
    if(auto conditionsNode = node["conditions"].as_table()){
        initializeConditions(*conditionsNode);
    }

    if(auto consumablesNode = node["consumables"].as_table()){
        initializeConsumables(*consumablesNode);
    }
}

bool Requirement::isMet(const std::vector<Skill>& skills, const std::vector<Item>& inventory, const std::vector<Reserve>& reserves) const{
    return checkConditions(skills, inventory, reserves) && checkConsumables(inventory, reserves);
}

bool Requirement::checkConditions(const std::vector<Skill>& skills, const std::vector<Item>& inventory, const std::vector<Reserve>& reserves) const{
    if(conditions.empty()){
        // Leaf node - check this specific condition
        if(type == "skill"){
            auto it = std::find_if(skills.begin(), skills.end(),
                                   [this](const Skill& skill) { return skill.name == this->name; });
            if (it != skills.end()) {
                return it->getLevel() >= this->level;
            }
            return false;
        }else if (type == "item"){
            auto it = std::find_if(inventory.begin(), inventory.end(),
                                   [this](const Item& item) { return item.name == this->name; });
            if (it != inventory.end()) {
                return it->count >= this->count;
            }
            return false;
        }else if (type == "reserve"){
            auto it = std::find_if(reserves.begin(), reserves.end(),
                                   [this](const Reserve& reserve) { return reserve.name == this->name; });
            if (it != reserves.end()) {
                return it->getCurrentValue() >= this->value;
            }
            return false;
        }
        return true; // Unknown type passes by default
    }

    // Logical operator node
    if(type == "AND"){
        for (const auto& cond : conditions) {
            if (!cond->checkConditions(skills, inventory, reserves)) return false;
        }
        return true;
    }else if (type == "OR"){
        for (const auto& cond : conditions) {
            if (cond->checkConditions(skills, inventory, reserves)) return true;
        }
        return false;
    }

    return true;
}

bool Requirement::checkConsumables(const std::vector<Item>& inventory, const std::vector<Reserve>& reserves) const {
    if(consumables.empty()){
        // Leaf node in consumables context
        if(type == "item"){
            auto it = std::find_if(inventory.begin(), inventory.end(),
                                   [this](const Item& item) { return item.name == this->name; });
            if(it != inventory.end()){
                return it->count >= this->count;
            }
            return false;
        }else if (type == "reserve"){
            auto it = std::find_if(reserves.begin(), reserves.end(),
                                   [this](const Reserve& reserve) { return reserve.name == this->name; });
            if(it != reserves.end()){
                return it->getCurrentValue() >= this->value;
            }
            return false;
        }
        return true; // Skills or unknown types in consumables pass (shouldn't happen)
    }

    // Logical operator node
    if(type == "AND"){
        for (const auto& cons : consumables){
            if (!cons->checkConsumables(inventory, reserves)) return false;
        }
        return true;
    }else if (type == "OR"){
        for(const auto& cons : consumables){
            if (cons->checkConsumables(inventory, reserves)) return true;
        }
        return false;
    }

    return true;
}

ConsumeList Requirement::getConsumeList(const std::vector<Item>& inventory, const std::vector<Reserve>& reserves) const{
    ConsumeList list;
    gatherConsumables(list, inventory, reserves);
    return list;
}

void Requirement::initializeConditions(const toml::table& node){
    this->type = *node["type"].value<std::string>();

    if(type == "AND" || type == "OR"){
        // Logical operator - parse nested conditions
        if(auto cond_list = node["conditions"].as_array()){
            for(const auto& cond : *cond_list) {
                if(auto table = cond.as_table()){
                    conditions.push_back(std::make_shared<Requirement>(*table));
                }
            }
        }
    }else{
        // Leaf node - parse specific fields
        name = node["name"].value_or("");
        level = node["level"].value_or(0);
        count = node["count"].value_or(0);
        value = node["value"].value_or(0.0);
    }
}

void Requirement::initializeConsumables(const toml::table& node){
    this->type = *node["type"].value<std::string>();

    if(type == "AND" || type == "OR"){
        // Parse nested consumables
        if(auto cons_list = node["consumables"].as_array()){
            for(const auto& cons : *cons_list){
                if(auto table = cons.as_table()){
                    consumables.push_back(std::make_shared<Requirement>(*table));
                }
            }
        }
    }else{
        // Leaf node - parse specific fields
        name = node["name"].value_or("");
        level = node["level"].value_or(0);
        count = node["count"].value_or(0);
        value = node["value"].value_or(0.0);
    }
}

void Requirement::gatherConsumables(ConsumeList& list, const std::vector<Item>& inventory, const std::vector<Reserve>& reserves) const{
    if(consumables.empty()){
        // Leaf node - add to consume list
        if(type == "item"){
            list.items.push_back({name, count});
        }else if (type == "reserve"){
            list.reserves.push_back({name, value});
        }
        // Skills are never consumed
        return;
    }

    // Logical operator node
    if(type == "AND"){
        // Consume all branches
        for(const auto& cons : consumables){
            cons->gatherConsumables(list, inventory, reserves);
        }
    }else if (type == "OR"){
        // Consume first available option
        for(const auto& cons : consumables){
            if(cons->checkConsumables(inventory, reserves)){
                cons->gatherConsumables(list, inventory, reserves);
                return; // Only consume first valid option
            }
        }
    }
}
