#ifndef SKILL_H
#define SKILL_H

#include<cmath>

class Skill
{
public:
    Skill(double baseXpForLevel, double scalingFactor);
    unsigned int level;     //level starts at 0
    double xp;
    double baseXpForLevel;
    double scalingFactor;   //xp to next level = baseXpForLevel * scalingFactor^level

    int getLevel();
    void addXp(double xp);

};

#endif // SKILL_H
