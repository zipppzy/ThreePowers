#ifndef COMBATENCOUNTER_H
#define COMBATENCOUNTER_H

#include "combatant.h"

class CombatEncounter
{
public:
    CombatEncounter(Combatant enemy);

    Combatant enemy;

    bool tickEnemy();     //ticks enemy's currentAttack; returns true when it completes (caller resolves damage + calls pickNextAttack)

    bool isEnemyDead() const;

private:
};

#endif // COMBATENCOUNTER_H