#ifndef REQUIREMENT_H
#define REQUIREMENT_H

//#include "player.h"
#include "item.h"
#include "skill.h"
#include "toml.hpp"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

// a tree that represents the requirements to do something
// type can be: "AND", "OR", "skill", "item"
// "AND"/"OR" nodes represent that relationship between all requirements in conditino
struct Requirement {
    std::string type;
    std::string name;
    int level;
    int count;
    std::vector<std::shared_ptr<Requirement>> conditions;

    explicit Requirement(const toml::table& node){
        type = *node["type"].value<std::string>();

        if (type == "AND" || type == "OR"){
            if (auto cond_list = node["conditions"].as_array()){
                for (const auto& cond : *cond_list){
                    conditions.push_back(std::make_shared<Requirement>(*cond.as_table()));
                }
            }
        }else if (type == "skill"){
            name = *node["name"].value<std::string>();
            level = *node["level"].value<int>();
        }else if (type == "item"){
            name = *node["name"].value<std::string>();
            count = *node["count"].value<int>();
        }
    }

    bool isMet(const std::vector<Skill>& skills, const std::vector<Item>& inventory) const{
        if (type == "AND"){
            for (const auto& cond : conditions){
                if (!cond->isMet(skills, inventory)) return false;
            }
            return true;
        }else if (type == "OR"){
            for (const auto& cond : conditions){
                if (cond->isMet(skills, inventory)) return true;
            }
            return false;
        }else if (type == "skill"){
            auto it  = std::find_if(skills.begin(), skills.end(), [&](const Skill& skill){return skill.name == this->name;});
            if(it != skills.end()){
                return it->getLevel() >= this->level;
            }
        }else if (type == "item"){
            auto it  = std::find_if(inventory.begin(), inventory.end(), [&](const Item& item){return item.name == this->name;});
            if(it != inventory.end()){
                return it->count >= this->count;
            }
        }
        return false;
    }
};

#endif // REQUIREMENT_H
