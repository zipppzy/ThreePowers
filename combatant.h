#ifndef COMBATANT_H
#define COMBATANT_H

#include "combatattack.h"
#include "toml.hpp"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// Lightweight representation of a combat participant on the enemy side.
// Does not reuse Player's Skill/Reserve/Action-queue machinery.
class Combatant
{
public:
    Combatant() = default;
    Combatant(const Combatant& other);
    Combatant(toml::table combatantTable);

    std::string name;
    double health;
    double maxHealth;
    double speed;

    std::shared_ptr<CombatAttack> currentAttack;

    bool tick();     //ticks currentAttack; returns true when it completes and is ready to resolve
    void pickNextAttack();
    void takeDamage(double amount);
    bool isDead() const;

    static void loadCombatantDatabase(const std::string& path);
    static std::optional<std::reference_wrapper<const Combatant>> checkCombatantDatabase(const std::string& name);

private:
    static std::unordered_map<std::string, Combatant> combatantDatabase;
    std::vector<std::pair<std::shared_ptr<CombatAttack>, double>> availableAttacks;   //attack, AI weight
};

#endif // COMBATANT_H