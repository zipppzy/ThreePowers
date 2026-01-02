#include "trigger.h"


TriggerContext::TriggerContext(const std::vector<Skill>* skills, const std::vector<Reserve>* reserves, const std::vector<Item>* inventory, const std::set<std::string>* visitedLocations)
    : skills(skills),
    reserves(reserves),
    inventory(inventory),
    visitedLocations(visitedLocations)
{}

TriggerCondition::TriggerCondition(const toml::table& node){
    type = node["type"].value_or("");

    if (type == "AND" || type == "OR" || type == "NOT") {
        // Logical operator - parse nested conditions
        if (auto cond_list = node["conditions"].as_array()) {
            for (const auto& cond : *cond_list) {
                if (auto table = cond.as_table()) {
                    conditions.push_back(std::make_shared<TriggerCondition>(*table));
                }
            }
        }
    } else {
        // Leaf condition - parse specific fields
        targetType = node["targetType"].value_or("");
        name = node["name"].value_or("");
        operation = node["operation"].value_or(">=");
        value = node["value"].value_or(0.0);
        count = node["count"].value_or(0);

    }
}

bool TriggerCondition::evaluate(const TriggerContext& context) const {
    if (type == "AND" || type == "OR" || type == "NOT") {
        return evaluateLogical(context);
    }
    return evaluateLeaf(context);
}

bool TriggerCondition::evaluateLogical(const TriggerContext& context) const {
    if (type == "AND") {
        for (const auto& cond : conditions) {
            if (!cond->evaluate(context)) return false;
        }
        return true;
    }

    if (type == "OR") {
        for (const auto& cond : conditions) {
            if (cond->evaluate(context)) return true;
        }
        return false;
    }

    if (type == "NOT") {
        if (conditions.empty()) return false;
        return !conditions[0]->evaluate(context);
    }

    return false;
}


bool TriggerCondition::evaluateLeaf(const TriggerContext& context) const {
    if (targetType == "skill") {
        if (!context.skills) return false;

        auto it = std::find_if(context.skills->begin(), context.skills->end(),
                               [this](const Skill& s) { return s.name == this->name; });
        if (it == context.skills->end()) return false;

        int level = it->getLevel();
        if (operation == ">=") return level >= static_cast<int>(value);
        if (operation == "<=") return level <= static_cast<int>(value);
        if (operation == "==") return level == static_cast<int>(value);
        if (operation == ">") return level > static_cast<int>(value);
        if (operation == "<") return level < static_cast<int>(value);
        if (operation == "!=") return level != static_cast<int>(value);
        return false;
    }

    if (targetType == "reserve") {
        if (!context.reserves) return false;

        auto it = std::find_if(context.reserves->begin(), context.reserves->end(),
                               [this](const Reserve& r) { return r.name == this->name; });
        if (it == context.reserves->end()) return false;

        double current = it->getCurrentValue();
        if (operation == ">=") return current >= value;
        if (operation == "<=") return current <= value;
        if (operation == "==") return current == value;
        if (operation == ">") return current > value;
        if (operation == "<") return current < value;
        if (operation == "!=") return current != value;
        return false;
    }

    if (targetType == "item") {
        if (!context.inventory) return false;

        auto it = std::find_if(context.inventory->begin(), context.inventory->end(),
                               [this](const Item& i) { return i.name == this->name; });
        if (it == context.inventory->end()) return false;

        if (operation == ">=") return it->count >= count;
        if (operation == "<=") return it->count <= count;
        if (operation == "==") return it->count == count;
        if (operation == ">") return it->count > count;
        if (operation == "<") return it->count < count;
        if (operation == "!=") return it->count != count;
        return false;
    }

    // if (targetType == "location") {
    //     std::string locationName = eventContext.empty() ? name : eventContext;

    //     if (operation == "visited") {
    //         if (!context.visitedLocations) return false;
    //         return context.visitedLocations->count(locationName) > 0;
    //     }

    //     if (operation == "current") {
    //         return context.currentLocationName == name;
    //     }

    //     if (operation == "visit" && firstTime) {
    //         if (!context.visitedLocations) return false;
    //         return context.visitedLocations->count(locationName) == 0 &&
    //                context.currentLocationName == locationName;
    //     }

    //     return false;
    // }
    if(targetType == "location"){
        if(operation == "visit"){
            return true;
        }
    }

    // if (targetType == "action") {
    //     if (!context.actionCompletionCounts) return false;
    //     std::string actionName = eventContext.empty() ? name : eventContext;

    //     if (operation == "completed") {
    //         auto it = context.actionCompletionCounts->find(actionName);
    //         if (it == context.actionCompletionCounts->end()) return false;
    //         return it->second >= count;
    //     }

    //     if (operation == "complete" && firstTime) {
    //         return context.actionCompletionCounts->count(actionName) == 0 &&
    //                actionName == eventContext;
    //     }

    //     return false;
    // }

    // if (targetType == "time") {
    //     if (operation == ">=") return context.ticks >= static_cast<unsigned long long>(value);
    //     if (operation == "<=") return context.ticks <= static_cast<unsigned long long>(value);
    //     if (operation == "==") return context.ticks == static_cast<unsigned long long>(value);
    //     if (operation == ">") return context.ticks > static_cast<unsigned long long>(value);
    //     if (operation == "<") return context.ticks < static_cast<unsigned long long>(value);
    //     if (operation == "!=") return context.ticks != static_cast<unsigned long long>(value);
    //     return false;
    // }

    return false;
}

Trigger::Trigger(const toml::table& node) {
    id = node["id"].value_or("");
    repeatable = node["repeatable"].value_or(false);
    priority = node["priority"].value_or(0);
    effectType = node["effectType"].value_or("");
    effectData = node["effectData"].value_or("");

    if (auto condTable = node["condition"].as_table()) {
        condition = std::make_shared<TriggerCondition>(*condTable);
    }
}
