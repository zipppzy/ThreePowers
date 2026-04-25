#include "researchtopic.h"

std::unordered_map<std::string, ResearchTopic> ResearchTopic::researchDatabase;

ResearchTopic::ResearchTopic(const toml::table& topicTable) {
    name                  = topicTable["name"].value_or("");
    description           = topicTable["description"].value_or("");
    attemptThreshold      = topicTable["attemptThreshold"].value_or(100.0);
    guaranteedThreshold   = topicTable["guaranteedThreshold"].value_or(500.0);
    priorAttemptReduction = topicTable["priorAttemptReduction"].value_or(10.0);

    if (auto effectsArray = topicTable["completionEffects"].as_array()) {
        for (const auto& effectNode : *effectsArray) {
            if (auto effectTable = effectNode.as_table()) {
                completionEffects.emplace_back(*effectTable);
            }
        }
    }
}

// --- Contribution formula ---
// Notes are sorted descending. Each note at position i contributes:
//   value * DECAY_RATE^i
// This rewards fewer high-value notes over many low-value ones.
double ResearchTopic::computeContribution(const std::vector<ResearchNote>& sortedNotes) {
    double total = 0.0;
    double decay = 1.0;
    for (const auto& note : sortedNotes) {
        total += note.knowledgeValue * decay;
        decay *= DECAY_RATE;
    }
    return total;
}

double ResearchTopic::tierContribution(int tier) const {
    auto it = notes.find(tier);
    if (it == notes.end() || it->second.empty()) return 0.0;
    return computeContribution(it->second);
}

double ResearchTopic::accumulatedKnowledge() const {
    double total = 0.0;
    for (const auto& [tier, noteList] : notes) {
        total += tierContribution(tier);
    }
    return total;
}

double ResearchTopic::currentGuaranteedThreshold() const {
    double reduced = guaranteedThreshold - priorAttempts * priorAttemptReduction;
    // Never drop below attemptThreshold + a small buffer so there's always some chance needed
    return std::max(reduced, attemptThreshold * 1.1);
}

double ResearchTopic::successChance() const {
    double knowledge = accumulatedKnowledge();
    if (knowledge < attemptThreshold) return 0.0;

    double guaranteed = currentGuaranteedThreshold();
    if (knowledge >= guaranteed) return 1.0;

    // Linear interpolation between BASE_ATTEMPT_SUCCESS at threshold and 1.0 at guaranteed
    double t = (knowledge - attemptThreshold) / (guaranteed - attemptThreshold);
    return BASE_ATTEMPT_SUCCESS + (1.0 - BASE_ATTEMPT_SUCCESS) * t;
}

void ResearchTopic::addNote(const ResearchNote& note) {
    auto& tierList = notes[note.tier];
    tierList.push_back(note);
    // Keep sorted descending by knowledgeValue so decay curve always optimal
    std::sort(tierList.begin(), tierList.end(),
              [](const ResearchNote& a, const ResearchNote& b) {
                  return a.knowledgeValue > b.knowledgeValue;
              });
}

bool ResearchTopic::attempt(double randomRoll) {
    bool success = randomRoll < successChance();

    // Clear all notes regardless of outcome - you've committed to the attempt
    notes.clear();

    if (success) {
        completed = true;
    } else {
        priorAttempts++;
    }

    return success;
}

std::optional<ResearchNote> ResearchTopic::merge(int tier, double focusSpent, double maxFocusCost) {
    auto it = notes.find(tier);
    if (it == notes.end() || it->second.empty()) return std::nullopt;

    auto& tierList = it->second;

    // Take up to MERGE_COUNT notes (already sorted descending, so these are the best ones)
    int count = std::min(static_cast<int>(tierList.size()), MERGE_COUNT);
    if (count < 2) return std::nullopt; // Need at least 2 to merge

    // Compute raw contribution of the notes being merged
    std::vector<ResearchNote> toMerge(tierList.begin(), tierList.begin() + count);
    double rawValue = computeContribution(toMerge);

    // Focus scales merge efficiency dramatically
    double focusRatio = std::clamp(focusSpent / maxFocusCost, 0.0, 1.0);
    double efficiency = BASE_MERGE_EFFICIENCY +
                        (MAX_MERGE_EFFICIENCY - BASE_MERGE_EFFICIENCY) * focusRatio;

    double mergedValue = rawValue * efficiency;

    // Remove the consumed notes
    tierList.erase(tierList.begin(), tierList.begin() + count);
    if (tierList.empty()) notes.erase(it);

    // Produce one note of the next tier
    ResearchNote result(mergedValue, tier + 1, name, "Merged " + ResearchNote::tierName(tier));
    return result;
}

void ResearchTopic::loadResearchDatabase(const std::string& path) {
    auto in = toml::parse_file(path);

    researchDatabase.clear();
    if (auto topicsArray = in["topics"].as_array()) {
        for (auto& elem : *topicsArray) {
            if (auto topicTable = elem.as_table()) {
                std::string topicName = topicTable->at("name").value_or("");
                researchDatabase.insert_or_assign(topicName, ResearchTopic(*topicTable));
            }
        }
    }
}

std::optional<std::reference_wrapper<const ResearchTopic>>
ResearchTopic::checkResearchDatabase(const std::string& name) {
    if (auto search = researchDatabase.find(name); search != researchDatabase.end()) {
        return std::cref(search->second);
    }
    return std::nullopt;
}
