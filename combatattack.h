#ifndef COMBATATTACK_H
#define COMBATATTACK_H

#include <optional>
#include <unordered_map>
#include "action.h"

class CombatAttack : public Action
{
public:
    enum DamageType{
        physical,
        qi,
        mana,
        psychic
    };

    enum AttackRange{
        melee,
        ranged,
        either
    };

    CombatAttack(std::string name, double baseDuration, DamageType damageType, double baseDamage, AttackRange range);
    CombatAttack(toml::table attackTable);

    DamageType getDamageType() const;
    double getBaseDamage() const;
    AttackRange getRange() const;

    static void loadAttackDatabase(const std::string& path);
    static std::optional<std::reference_wrapper<const CombatAttack>> checkAttackDatabase(const std::string& name);

    static std::optional<DamageType> damageTypeFromString(const std::string& s);
    static std::optional<AttackRange> attackRangeFromString(const std::string& s);

private:
    static std::unordered_map<std::string, CombatAttack> attackDatabase;

    DamageType damageType;
    double baseDamage;
    AttackRange range;
};

#endif // COMBATATTACK_H
