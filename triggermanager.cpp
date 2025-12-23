// triggermanager.cpp
#include "triggermanager.h"
#include <algorithm>

void TriggerManager::loadTriggers(const std::string& path) {
    auto in = toml::parse_file(path);

    if (auto triggersArray = in["triggers"].as_array()) {
        for (auto& elem : *triggersArray) {
            if (auto triggerTable = elem.as_table()) {
                triggers.emplace_back(*triggerTable);
            }
        }
    }

    std::sort(triggers.begin(), triggers.end(),
              [](const Trigger& a, const Trigger& b) { return a.priority > b.priority; });

    buildIndices();
}

void TriggerManager::buildIndices() {
    locationTriggers.clear();
    actionTriggers.clear();
    skillTriggers.clear();
    periodicTriggers.clear();

    for (size_t i = 0; i < triggers.size(); ++i) {
        if (triggers[i].condition) {
            indexTrigger(i, triggers[i].condition.get());
        }
    }
}

void TriggerManager::indexTrigger(size_t index, const TriggerCondition* condition) {
    if (!condition) return;

    if (condition->type == "AND" || condition->type == "OR" || condition->type == "NOT") {
        if (needsPeriodicCheck(condition)) {
            periodicTriggers.push_back(index);
        }
        for (const auto& subCond : condition->conditions) {
            indexTrigger(index, subCond.get());
        }
        return;
    }

    if (condition->targetType == "location") {
        locationTriggers[condition->name].push_back(index);
    } else if (condition->targetType == "action") {
        actionTriggers[condition->name].push_back(index);
    } else if (condition->targetType == "skill") {
        skillTriggers[condition->name].push_back(index);
    } else if (condition->targetType == "time") {
        periodicTriggers.push_back(index);
    }
}

bool TriggerManager::needsPeriodicCheck(const TriggerCondition* condition) const {
    if (!condition) return false;
    if (condition->targetType == "time") return true;

    for (const auto& subCond : condition->conditions) {
        if (needsPeriodicCheck(subCond.get())) return true;
    }
    return false;
}

void TriggerManager::recordLocationVisit(const std::string& locationName) {
    visitedLocations.insert(locationName);
}

void TriggerManager::recordActionCompletion(const std::string& actionName) {
    actionCompletionCounts[actionName]++;
}

void TriggerManager::checkTriggers(const std::vector<size_t>& indices, const TriggerContext& context, const std::string& eventContext) {
    for (size_t idx : indices) {
        if (idx >= triggers.size()) continue;

        Trigger& trigger = triggers[idx];
        if (trigger.checkCondition(context)) {
            effectQueue.push({trigger.effectType, trigger.effectData, trigger.priority});
            trigger.markTriggered();
        }
    }
}

void TriggerManager::onLocationChange(const TriggerContext& context, const std::string& locationName) {
    auto it = locationTriggers.find(locationName);
    if (it != locationTriggers.end()) {
        checkTriggers(it->second, context, locationName);
    }
}

void TriggerManager::onActionComplete(const TriggerContext& context, const std::string& actionName) {
    auto it = actionTriggers.find(actionName);
    if (it != actionTriggers.end()) {
        checkTriggers(it->second, context, actionName);
    }
}

void TriggerManager::onSkillLevelUp(const TriggerContext& context, const std::string& skillName) {
    auto it = skillTriggers.find(skillName);
    if (it != skillTriggers.end()) {
        checkTriggers(it->second, context, skillName);
    }
}

void TriggerManager::checkPeriodicTriggers(const TriggerContext& context) {
    checkTriggers(periodicTriggers, context);
}

std::optional<TriggerManager::TriggeredEffect> TriggerManager::getNextEffect() {
    if (effectQueue.empty()) return std::nullopt;
    TriggeredEffect effect = effectQueue.top();
    effectQueue.pop();
    return effect;
}

void TriggerManager::saveState(toml::table& table) const {
    toml::array triggeredArray;
    for (const auto& trigger : triggers) {
        if (trigger.triggered) {
            triggeredArray.push_back(trigger.id);
        }
    }
    table.insert("triggeredTriggers", triggeredArray);

    toml::array locationsArray;
    for (const auto& loc : visitedLocations) {
        locationsArray.push_back(loc);
    }
    table.insert("visitedLocations", locationsArray);

    toml::table completionsTable;
    for (const auto& [actionName, count] : actionCompletionCounts) {
        completionsTable.insert(actionName, count);
    }
    table.insert("actionCompletions", completionsTable);
}

void TriggerManager::loadState(const toml::table& table) {
    if (auto triggeredArray = table["triggeredTriggers"].as_array()) {
        std::unordered_set<std::string> triggeredIds;
        for (const auto& elem : *triggeredArray) {
            if (auto id = elem.value<std::string>()) {
                triggeredIds.insert(*id);
            }
        }
        for (auto& trigger : triggers) {
            if (triggeredIds.count(trigger.id)) {
                trigger.markTriggered();
            }
        }
    }

    visitedLocations.clear();
    if (auto locationsArray = table["visitedLocations"].as_array()) {
        for (const auto& elem : *locationsArray) {
            if (auto loc = elem.value<std::string>()) {
                visitedLocations.insert(*loc);
            }
        }
    }

    actionCompletionCounts.clear();
    if (auto completionsTable = table["actionCompletions"].as_table()) {
        for (const auto& [key, value] : *completionsTable) {
            if (auto count = value.as_integer()) {
                actionCompletionCounts[std::string(key)] = count->get();
            }
        }
    }
}
