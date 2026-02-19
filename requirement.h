#ifndef REQUIREMENT_H
#define REQUIREMENT_H

#include "item.h"
#include "reserve.h"
#include "skill.h"
#include "toml.hpp"
#include <algorithm>
#include <memory>
#include <QString>
#include <string>
#include <vector>

// What needs to be consumed when requirement is fulfilled
// What needs to be consumed when requirement is fulfilled
struct ConsumeList {
    std::vector<std::pair<std::string, int>> items;        // name, count
    std::vector<std::pair<std::string, double>> reserves;  // name, amount
};

// A tree that represents requirements - SIMPLIFIED to single tree
// Use separate instances for conditions vs consumables
struct Requirement {
    std::string type;
    std::string name;
    int level;
    int count;
    double value;

    std::vector<std::shared_ptr<Requirement>> children;   // Child requirements (for AND/OR nodes)

    explicit Requirement(const toml::table& node);

    // Check if all requirements are met
    bool isMet(const std::vector<Skill>& skills, const std::vector<Item>& inventory, const std::vector<Reserve>& reserves) const;

    // Get what should be consumed (only makes sense for consumables tree)
    ConsumeList getConsumeList(const std::vector<Item>& inventory, const std::vector<Reserve>& reserves) const;

    // Generate formatted string for display in tooltips
    QString toDisplayString(const std::vector<Skill>& skills, const std::vector<Item>& inventory, const std::vector<Reserve>& reserves) const;

private:
    // Helper to recursively build display string
    QString buildDisplayString(const std::vector<Skill>& skills, const std::vector<Item>& inventory, const std::vector<Reserve>& reserves, int indent) const;

    // Helper to recursively get consumables from tree
    void gatherConsumables(ConsumeList& list, const std::vector<Item>& inventory, const std::vector<Reserve>& reserves) const;

    // Check if a leaf node's requirements are met
    bool checkLeafRequirement(const std::vector<Skill>& skills, const std::vector<Item>& inventory, const std::vector<Reserve>& reserves) const;
};


#endif // REQUIREMENT_H
