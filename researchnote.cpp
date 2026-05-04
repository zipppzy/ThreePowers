#include "researchnote.h"

ResearchNote::ResearchNote(double knowledgeValue, int tier, const std::string& topicName)
    : knowledgeValue(knowledgeValue)
    , tier(tier)
    , topicName(topicName)
{}

ResearchNote::ResearchNote(const toml::table& noteTable) {
    knowledgeValue = noteTable["knowledgeValue"].value_or(0.0);
    tier           = noteTable["tier"].value_or(0);
    topicName      = noteTable["topicName"].value_or("");
}

std::string ResearchNote::tierName(int tier) {
    switch (tier) {
    case 0:  return "Thought";
    case 1:  return "Idea";
    case 2:  return "Theory";
    case 3:  return "Insight";
    case 4:  return "Revelation";
    default: return "Revelation"; // cap at max
    }
}
