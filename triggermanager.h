// triggermanager.h
#ifndef TRIGGERMANAGER_H
#define TRIGGERMANAGER_H

#include "trigger.h"
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <optional>

class TriggerManager {
public:
    struct TriggeredEffect {
        std::string effectType;
        std::string effectData;
        int priority;

        bool operator<(const TriggeredEffect& other) const {
            return priority < other.priority;
        }
    };

    TriggerManager() = default;

    void loadTriggers(const std::string& path);

    // Tracking
    void recordLocationVisit(const std::string& locationName);
    void recordActionCompletion(const std::string& actionName);

    // Event-driven evaluation
    void onLocationChange(const TriggerContext& context, const std::string& locationName);
    void onActionComplete(const TriggerContext& context, const std::string& actionName);
    void onSkillLevelUp(const TriggerContext& context, const std::string& skillName);

    // Periodic checks (for time-based triggers)
    void checkPeriodicTriggers(const TriggerContext& context);

    // Queue access
    bool hasPendingEffects() const { return !effectQueue.empty(); }  // Changed from hasQueuedActions
    std::optional<TriggeredEffect> getNextEffect();  // Changed from getNextAction

    // State access (for building context)
    const std::unordered_set<std::string>& getVisitedLocations() const { return visitedLocations; }
    const std::unordered_map<std::string, int>& getActionCompletionCounts() const { return actionCompletionCounts; }

    // Save/load
    void saveState(toml::table& table) const;
    void loadState(const toml::table& table);

private:
    std::vector<Trigger> triggers;
    std::priority_queue<TriggeredEffect> effectQueue;  // Changed from actionQueue

    // Indexed triggers
    std::unordered_map<std::string, std::vector<size_t>> locationTriggers;
    std::unordered_map<std::string, std::vector<size_t>> actionTriggers;
    std::unordered_map<std::string, std::vector<size_t>> skillTriggers;
    std::vector<size_t> periodicTriggers;

    // Tracking data
    std::unordered_set<std::string> visitedLocations;
    std::unordered_map<std::string, int> actionCompletionCounts;

    void buildIndices();
    void indexTrigger(size_t index, const TriggerCondition* condition);
    void checkTriggers(const std::vector<size_t>& indices, const TriggerContext& context, const std::string& eventContext = "");
    bool needsPeriodicCheck(const TriggerCondition* condition) const;
};

#endif // TRIGGERMANAGER_H
