#ifndef SKILL_H
#define SKILL_H

#include "effect.h"
#include "toml.hpp"
#include "logger.h"
#include<cmath>
#include <string>
#include <unordered_map>

class Skill
{
public:
    Skill();
    Skill(std::string name, double baseXpForLevel, double scalingFactor, unsigned int maxLevel);
    Skill(std::string name, double baseXpForLevel, double scalingFactor, unsigned int level, double xp, unsigned int maxLevel);
    Skill(toml::table skillTable);

    std::string name;
    unsigned int getLevel() const;
    double getXp() const;
    double getXpToNextLevel() const;
    void addXp(double xp);
    unsigned int getMaxLevel() const;
    toml::table getSkillTable() const;
    std::optional<std::reference_wrapper<const std::vector<Effect>>> checkEffects(const std::string& name) const;
    static void loadSkillDatabase(std::string path);
    static std::optional<std::reference_wrapper<const Skill>> checkSkillDatabase(std::string name);

private:
    static std::unordered_map<std::string, Skill> skillDatabase;

    std::unordered_map<std::string, std::vector<Effect>> effects;   //name of action or reserve : list of effects
    unsigned int level;         //level starts at 0
    unsigned int maxLevel;      //gain no more xp after reaching this level
    double xp;
    double baseXpForLevel;
    double scalingFactor;   //xp to next level = baseXpForLevel * scalingFactor^level

};

#endif // SKILL_H
