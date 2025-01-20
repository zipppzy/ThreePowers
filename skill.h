#ifndef SKILL_H
#define SKILL_H

#include "toml.hpp"
#include<cmath>
#include <string>
#include <unordered_map>

class Skill
{
public:
    Skill();
    Skill(std::string name, double baseXpForLevel, double scalingFactor);
    Skill(std::string name, double baseXpForLevel, double scalingFactor, unsigned int level, double xp);
    Skill(toml::table skillTable);

    std::string name;
    unsigned int getLevel();
    void addXp(double xp);
    toml::table getSkillTable();
    static void loadSkillDatabase(std::string path);
    static std::optional<std::reference_wrapper<const Skill>> checkSkillDatabase(std::string name);

private:
    static std::unordered_map<std::string, Skill> skillDatabase;
    unsigned int level;         //level starts at 0
    unsigned int maxLevel;      //gain no more xp after reaching this level
    double xp;
    double baseXpForLevel;
    double scalingFactor;   //xp to next level = baseXpForLevel * scalingFactor^level

};

#endif // SKILL_H
