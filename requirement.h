#ifndef REQUIREMENT_H
#define REQUIREMENT_H

//#include "player.h"
#include "item.h"
#include "reserve.h"
#include "skill.h"
#include "toml.hpp"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

// a tree that represents the requirements to do something
// type can be: "AND", "OR", "skill", "item", "reserve"
// "AND"/"OR" nodes represent that relationship between all requirements in conditions
// skill has level, item has count, and reserve has value
// if(consume) indicates that that requirement should be consumed when requirement is fufilled
// skill should not consume
struct Requirement {
    std::string type;
    std::string name;
    int level;
    int count;
    double value;
    bool consume;
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
            consume = node["consume"].value_or(false);
        }else if (type == "item"){
            name = *node["name"].value<std::string>();
            count = *node["count"].value<int>();
        }else if (type == "reserve"){
            name = *node["name"].value<std::string>();
            count = *node["value"].value<double>();
        }
    }

    bool isMet(const std::vector<Skill>& skills, const std::vector<Item>& inventory, const std::vector<Reserve> reserves) const{
        if (type == "AND"){
            for (const auto& cond : conditions){
                if (!cond->isMet(skills, inventory, reserves)) return false;
            }
            return true;
        }else if (type == "OR"){
            for (const auto& cond : conditions){
                if (cond->isMet(skills, inventory, reserves)) return true;
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
        }else if (type == "item"){
            auto it  = std::find_if(reserves.begin(), reserves.end(), [&](const Reserve& reserve){return reserve.name == this->name;});
            if(it != reserves.end()){
                return it->getCurrentValue() >= this->value;
            }
        }
        return false;
    }
    //Total mess prob rework
    // void getConsumeOptions(
    //     const Requirement& req,
    //     std::vector<std::vector<std::shared_ptr<Requirement>>>& result,
    //     std::vector<std::shared_ptr<Requirement>> current = {}
    //     ) {
    //     if (req.type == "AND") {
    //         std::vector<std::vector<std::shared_ptr<Requirement>>> partialResults;
    //         for (const auto& cond : req.conditions) {
    //             std::vector<std::vector<std::shared_ptr<Requirement>>> subResults;
    //             getConsumeOptions(*cond, subResults, {});
    //             if(subResults.empty()){
    //                 partialResults.push_back({ {} });
    //             }else{
    //                 partialResults.push_back(subResults);
    //             }
    //         }

    //         std::vector<std::shared_ptr<Requirement>> temp;
    //         std::function<void(size_t)> combine = [&](size_t index) {
    //             if (index == partialResults.size()) {
    //                 result.push_back(temp);
    //                 return;
    //             }
    //             for (const auto& subset : partialResults[index]) {
    //                 temp.insert(temp.end(), subset.begin(), subset.end());
    //                 combine(index + 1);
    //                 temp.erase(temp.end() - subset.size(), temp.end());
    //             }
    //         };
    //         combine(0);
    //     }
    //     else if (req.type == "OR") {
    //         for (const auto& cond : req.conditions) {
    //             getConsumeOptions(*cond, result, current);
    //         }
    //     }
    //     else if (req.consume) {
    //         current.push_back(std::make_shared<Requirement>(req));
    //         result.push_back(current);
    //     }
    // }

};

#endif // REQUIREMENT_H
