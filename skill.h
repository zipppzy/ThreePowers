#ifndef SKILL_H
#define SKILL_H

#include "toml.hpp"
#include<cmath>
#include <string>

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

private:
    unsigned int level;     //level starts at 0
    double xp;
    double baseXpForLevel;
    double scalingFactor;   //xp to next level = baseXpForLevel * scalingFactor^level

};

#endif // SKILL_H
