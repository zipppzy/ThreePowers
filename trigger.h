#ifndef TRIGGER_H
#define TRIGGER_H

#include "instanteffect.h"
#include "item.h"
#include "reserve.h"
#include "skill.h"
#include "toml.hpp"
#include <set>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

// Lightweight context about player for evaluation
struct TriggerContext
{
    const std::vector<Skill>* skills = nullptr;
    //probably remove reserves as a trigger type
    const std::vector<Reserve>* reserves = nullptr;
    const std::vector<Item>* inventory = nullptr;
    const std::set<std::string>* visitedLocations = nullptr;

    TriggerContext() = default;
    TriggerContext(const std::vector<Skill>* skills, const std::vector<Reserve>* reserves, const std::vector<Item>* inventory, const std::set<std::string>* visitedLocations);
};

struct TriggerCondition
{

    std::string type;  // "AND", "OR", "NOT", or leaf types

    // Leaf condition data
    std::string targetType;    // "skill", "reserve", "item", "location", "action", "time"
    std::string name;
    std::string operation;     // ">=", "<=", "==", ">", "<", "!=", "visited", "completed", "current"
    double value = 0.0;
    int count = 0;


    // Nested conditions for logical operators
    std::vector<std::shared_ptr<TriggerCondition>> conditions;

    explicit TriggerCondition(const toml::table& node);

    bool evaluate(const TriggerContext& context) const;

private:
    bool evaluateLogical(const TriggerContext& context) const;
    bool evaluateLeaf(const TriggerContext& context) const;
};

struct Trigger {
    std::string id;
    std::shared_ptr<TriggerCondition> condition;
    std::vector<InstantEffect> effects;
    bool repeatable = false;
    bool triggered = false;
    int priority = 0;

    Trigger() = default;
    explicit Trigger(const toml::table& node);

    bool checkCondition(const TriggerContext& context) const {
        if (!repeatable && triggered) return false;
        if (!condition) return false;
        return condition->evaluate(context);
    }

    void markTriggered() { triggered = true; }
    void reset() { triggered = false; }
};

#endif // TRIGGER_H
