#include "combatencounter.h"

CombatEncounter::CombatEncounter(Combatant enemy)
    : enemy(enemy)
{}

bool CombatEncounter::tickEnemy(){
    if(this->enemy.isDead()) return false;
    return this->enemy.tick();
}

bool CombatEncounter::isEnemyDead() const{
    return this->enemy.isDead();
}