#ifndef REQUIREMENT_H
#define REQUIREMENT_H

#include "item.h"
#include "reserve.h"
#include "skill.h"
#include "toml.hpp"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

// What needs to be consumed when requirement is fulfilled
struct ConsumeList {
    std::vector<std::pair<std::string, int>> items;        // name, count
    std::vector<std::pair<std::string, double>> reserves;  // name, amount
};

// A tree that represents the requirements to do something
// Separated into conditions (check-only) and consumables (actually consumed)
struct Requirement {
    std::string type;
    std::string name;
    int level;
    int count;
    double value;

    std::vector<std::shared_ptr<Requirement>> conditions;    // Check-only requirements
    std::vector<std::shared_ptr<Requirement>> consumables;   // Items/reserves to consume

    explicit Requirement(const toml::table& node);

    // Check if all requirements (both conditions and consumables) are met
    bool isMet(const std::vector<Skill>& skills, const std::vector<Item>& inventory, const std::vector<Reserve>& reserves) const;

    // Get what should be consumed (only processes consumables tree)
    ConsumeList getConsumeList(const std::vector<Item>& inventory, const std::vector<Reserve>& reserves) const;

private:
    //helper functions for constructor
    void initializeConditions(const toml::table& node);
    void initializeConsumables(const toml::table& node);
    bool checkConditions(const std::vector<Skill>& skills, const std::vector<Item>& inventory, const std::vector<Reserve>& reserves) const;
    bool checkConsumables(const std::vector<Item>& inventory, const std::vector<Reserve>& reserves) const;

    // Helper to recursively get consumables from tree
    void gatherConsumables(ConsumeList& list, const std::vector<Item>& inventory, const std::vector<Reserve>& reserves) const;
};

#endif // REQUIREMENT_H
