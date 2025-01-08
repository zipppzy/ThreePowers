#include "skill.h"

Skill::Skill(){};

Skill::Skill(std::string name, double baseXpForLevel, double scalingFactor){
    this->name = name;
    this->baseXpForLevel = baseXpForLevel;
    this->scalingFactor = scalingFactor;
    this->level = 0;
    this->xp = 0;
}

Skill::Skill(std::string name, double baseXpForLevel, double scalingFactor, unsigned int level, double xp){
    this->name = name;
    this->baseXpForLevel = baseXpForLevel;
    this->scalingFactor = scalingFactor;
    this->level = level;
    this->xp = xp;
}
unsigned int Skill::getLevel(){
    return this->level;
}

void Skill::addXp(double xp){
    this->xp += xp;
    double xpToLevel = this->baseXpForLevel*std::pow(this->scalingFactor, this->level);
    if(this->xp > xpToLevel){
        this->xp -=xpToLevel;
        this->level += 1;
    }
}

toml::table Skill::getSkillTable(){
    toml::table skill;
    skill.insert("name", name);
    skill.insert("level", level);
    skill.insert("xp", xp);
    skill.insert("baseXpForLevel", baseXpForLevel);
    skill.insert("scalingFactor", scalingFactor);
    return skill;
}
