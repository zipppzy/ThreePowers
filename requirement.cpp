#include "requirement.h"

Requirement::Requirement(const toml::table& node) {
    this->type = node["type"].value_or("");

    if (type == "AND" || type == "OR") {
        // Logical operator - parse nested children
        if (auto child_list = node["children"].as_array()) {
            for (const auto& child : *child_list) {
                if (auto table = child.as_table()) {
                    children.push_back(std::make_shared<Requirement>(*table));
                }
            }
        }
        // Also support old format with "conditions" or "consumables" arrays
        if (auto cond_list = node["conditions"].as_array()) {
            for (const auto& child : *cond_list) {
                if (auto table = child.as_table()) {
                    children.push_back(std::make_shared<Requirement>(*table));
                }
            }
        }
        if (auto cons_list = node["consumables"].as_array()) {
            for (const auto& child : *cons_list) {
                if (auto table = child.as_table()) {
                    children.push_back(std::make_shared<Requirement>(*table));
                }
            }
        }
    } else {
        // Leaf node - parse specific fields
        name = node["name"].value_or("");
        level = node["level"].value_or(0);
        count = node["count"].value_or(0);
        value = node["value"].value_or(0.0);
    }
}

bool Requirement::isMet(const std::vector<Skill>& skills,
                        const std::vector<Item>& inventory,
                        const std::vector<Reserve>& reserves) const {
    if (children.empty()) {
        // Leaf node - check this specific requirement
        return checkLeafRequirement(skills, inventory, reserves);
    }

    // Logical operator node
    if (type == "AND") {
        for (const auto& child : children) {
            if (!child->isMet(skills, inventory, reserves)) return false;
        }
        return true;
    } else if (type == "OR") {
        for (const auto& child : children) {
            if (child->isMet(skills, inventory, reserves)) return true;
        }
        return false;
    }

    return true;
}

bool Requirement::checkLeafRequirement(const std::vector<Skill>& skills,
                                       const std::vector<Item>& inventory,
                                       const std::vector<Reserve>& reserves) const {
    if (type == "skill") {
        auto it = std::find_if(skills.begin(), skills.end(),
                               [this](const Skill& skill) { return skill.name == this->name; });
        if (it != skills.end()) {
            return it->getLevel() >= this->level;
        }
        return false;
    } else if (type == "item") {
        auto it = std::find_if(inventory.begin(), inventory.end(),
                               [this](const Item& item) { return item.name == this->name; });
        if (it != inventory.end()) {
            return it->count >= this->count;
        }
        return false;
    } else if (type == "reserve") {
        auto it = std::find_if(reserves.begin(), reserves.end(),
                               [this](const Reserve& reserve) { return reserve.name == this->name; });
        if (it != reserves.end()) {
            return it->getCurrentValue() >= this->value;
        }
        return false;
    }

    return true; // Unknown type passes by default
}

ConsumeList Requirement::getConsumeList(const std::vector<Item>& inventory,
                                        const std::vector<Reserve>& reserves) const {
    ConsumeList list;
    gatherConsumables(list, inventory, reserves);
    return list;
}

void Requirement::gatherConsumables(ConsumeList& list,
                                    const std::vector<Item>& inventory,
                                    const std::vector<Reserve>& reserves) const {
    if (children.empty()) {
        // Leaf node - add to consume list
        if (type == "item") {
            list.items.push_back({name, count});
        } else if (type == "reserve") {
            list.reserves.push_back({name, value});
        }
        // Skills are never consumed
        return;
    }

    // Logical operator node
    if (type == "AND") {
        // Consume all branches
        for (const auto& child : children) {
            child->gatherConsumables(list, inventory, reserves);
        }
    } else if (type == "OR") {
        // Consume first available option
        for (const auto& child : children) {
            if (child->isMet({}, inventory, reserves)) {  // Check if this option is available
                child->gatherConsumables(list, inventory, reserves);
                return; // Only consume first valid option
            }
        }
    }
}

QString Requirement::toDisplayString(const std::vector<Skill>& skills,
                                     const std::vector<Item>& inventory,
                                     const std::vector<Reserve>& reserves) const {
    return buildDisplayString(skills, inventory, reserves, 0);
}

QString Requirement::buildDisplayString(const std::vector<Skill>& skills,
                                        const std::vector<Item>& inventory,
                                        const std::vector<Reserve>& reserves,
                                        int indent) const {
    QString indentStr = QString(indent * 2, ' ');
    QString result;

    if (children.empty()) {
        // Leaf node - display the requirement
        if (type.empty() || type == "") {
            return "";
        }

        bool met = checkLeafRequirement(skills, inventory, reserves);
        QString status = met ? "✓" : "✗";
        QString color = met ? "#00ff00" : "#ff0000";  // Green if met, red if not

        if (type == "skill") {
            auto it = std::find_if(skills.begin(), skills.end(),
                                   [this](const Skill& skill) { return skill.name == this->name; });
            int currentLevel = (it != skills.end()) ? it->getLevel() : 0;
            result = QString("<span style='color:%1'>%2 %3: Level %4 (have %5)</span>")
                         .arg(color)
                         .arg(status)
                         .arg(QString::fromStdString(name))
                         .arg(level)
                         .arg(currentLevel);
        } else if (type == "item") {
            auto it = std::find_if(inventory.begin(), inventory.end(),
                                   [this](const Item& item) { return item.name == this->name; });
            int currentCount = (it != inventory.end()) ? it->count : 0;
            result = QString("<span style='color:%1'>%2 %3 x%4 (have %5)</span>")
                         .arg(color)
                         .arg(status)
                         .arg(QString::fromStdString(name))
                         .arg(count)
                         .arg(currentCount);
        } else if (type == "reserve") {
            auto it = std::find_if(reserves.begin(), reserves.end(),
                                   [this](const Reserve& reserve) { return reserve.name == this->name; });
            double currentValue = (it != reserves.end()) ? it->getCurrentValue() : 0.0;
            result = QString("<span style='color:%1'>%2 %3: %4 (have %5)</span>")
                         .arg(color)
                         .arg(status)
                         .arg(QString::fromStdString(name))
                         .arg(value)
                         .arg(currentValue, 0, 'f', 1);
        }

        return indentStr + result;
    }

    // Logical operator node with children
    result += indentStr + QString::fromStdString(type) + ":\n";
    for (size_t i = 0; i < children.size(); i++) {
        result += children[i]->buildDisplayString(skills, inventory, reserves, indent + 1);
        if (i < children.size() - 1) {
            result += "\n";
        }
    }

    return result;
}
