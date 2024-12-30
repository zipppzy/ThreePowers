#include "skill.h"

Skill::Skill(double baseXpForLevel, double scalingFactor){
    this->baseXpForLevel = baseXpForLevel;
    this->scalingFactor = scalingFactor;
}

int Skill::getLevel(){
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
