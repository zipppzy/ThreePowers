#ifndef RESEARCHTOPIC_H
#define RESEARCHTOPIC_H

#include "researchnote.h"
#include "instanteffect.h"
#include "toml.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <algorithm>
#include <cmath>

class ResearchTopic {
public:
    ResearchTopic() = default;
    ResearchTopic(const toml::table& topicTable);

    // Identity
    std::string name;
    std::string description;

    // Knowledge thresholds
    double attemptThreshold;        // minimum knowledge to make an attempt
    double guaranteedThreshold;     // knowledge required for 100% success chance
    double priorAttemptReduction;   // how much guaranteedThreshold drops per failed attempt

    // State
    int priorAttempts = 0;
    bool completed = false;

    // Notes owned by this topic, stored per tier
    // Key = tier, value = notes sorted descending by knowledgeValue (auto-maintained)
    std::unordered_map<int, std::vector<ResearchNote>> notes;

    // Payoff on completion
    std::vector<InstantEffect> completionEffects;

    // --- Core calculations ---

    // Total accumulated progress toward the attempt/guaranteed thresholds
    double accumulatedKnowledge() const;

    // Success chance for an attempt right now (0.0 - 1.0); 0 if below attemptThreshold
    double successChance() const;

    // Current guaranteed threshold accounting for prior attempts
    double currentGuaranteedThreshold() const;

    // Add a note; keeps tier sorted descending automatically
    void addNote(const ResearchNote& note);

    // Attempt the research cashout; returns true on success, updates state on failure
    // Clears all notes on attempt regardless of outcome
    bool attempt(double randomRoll);    // randomRoll in [0, 1)

    // --- Merge helpers ---

    // Compute the knowledge value of merging up to MERGE_COUNT notes from the given tier.
    // focusSpent in [0, maxFocusCost] scales merge efficiency.
    // Returns the resulting note value and removes the consumed notes from this topic.
    std::optional<ResearchNote> merge(int tier, double focusSpent, double maxFocusCost);

    // How much knowledge the notes in a tier contribute to accumulatedKnowledge()
    double tierContribution(int tier) const;

    // Database
    static void loadResearchDatabase(const std::string& path);
    static std::optional<std::reference_wrapper<const ResearchTopic>> checkResearchDatabase(const std::string& name);

    // Tuning constants
    static constexpr int    MERGE_COUNT          = 10;
    static constexpr double DECAY_RATE           = 0.85;   // per-note decay within a tier
    static constexpr double BASE_MERGE_EFFICIENCY  = 0.05;  // without focus: very lossy
    static constexpr double MAX_MERGE_EFFICIENCY   = 0.85;  // with full focus
    static constexpr double BASE_ATTEMPT_SUCCESS   = 0.20;  // success chance at attemptThreshold

private:
    static std::unordered_map<std::string, ResearchTopic> researchDatabase;

    // Compute contribution of a sorted (descending) list of note values using decay curve
    static double computeContribution(const std::vector<ResearchNote>& sortedNotes);
};

#endif // RESEARCHTOPIC_H
