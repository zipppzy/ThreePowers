#ifndef RESEARCHNOTE_H
#define RESEARCHNOTE_H

#include "toml.hpp"
#include <string>

class ResearchNote {
public:
    ResearchNote() = default;
    ResearchNote(double knowledgeValue, int tier, const std::string& topicName, const std::string& sourceName = "");
    ResearchNote(const toml::table& noteTable);

    double knowledgeValue;
    int tier;               // 0 = thought, 1 = idea, 2 = theory (and beyond)
    std::string topicName;  // which research topic this note belongs to
    std::string sourceName; // flavor: where this knowledge came from

    // Tier name helpers
    static std::string tierName(int tier);
    static constexpr int MAX_TIER = 4;
};

#endif // RESEARCHNOTE_H
