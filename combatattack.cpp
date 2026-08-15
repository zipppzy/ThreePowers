#include "combatattack.h"

CombatAttack::CombatAttack(std::string name, double baseDuration, DamageType damageType, double baseDamage, AttackRange range)
    : Action(name, baseDuration)
    , damageType(damageType)
    , baseDamage(baseDamage)
    , range(range)
{}

CombatAttack::CombatAttack(toml::table attackTable)
    : Action(attackTable)
{
    std::string damageTypeStr = attackTable["damageType"].value_or("");
    this->damageType = CombatAttack::damageTypeFromString(damageTypeStr).value_or(physical);

    this->baseDamage = attackTable["baseDamage"].value_or(0.0);

    std::string rangeStr = attackTable["range"].value_or("");
    this->range = CombatAttack::attackRangeFromString(rangeStr).value_or(melee);
}

CombatAttack::DamageType CombatAttack::getDamageType() const{
    return this->damageType;
}

double CombatAttack::getBaseDamage() const{
    return this->baseDamage;
}

CombatAttack::AttackRange CombatAttack::getRange() const{
    return this->range;
}

std::unordered_map<std::string, CombatAttack> CombatAttack::attackDatabase;

void CombatAttack::loadAttackDatabase(const std::string& path){
    auto in = toml::parse_file(path);

    CombatAttack::attackDatabase.clear();
    if(auto attacksArray = in["attacks"].as_array()){
        for(auto& elem : *attacksArray){
            if(auto attackTable = elem.as_table()){
                CombatAttack::attackDatabase.insert_or_assign(attackTable->at("name").value_or(""), CombatAttack(*attackTable));
            }
        }
    }
}

std::optional<std::reference_wrapper<const CombatAttack>> CombatAttack::checkAttackDatabase(const std::string& name){
    if(auto search = CombatAttack::attackDatabase.find(name); search != CombatAttack::attackDatabase.end()){
        return std::cref(search->second);
    }else{
        return std::nullopt;
    }
}

std::optional<CombatAttack::DamageType> CombatAttack::damageTypeFromString(const std::string& s){
    static const std::unordered_map<std::string, CombatAttack::DamageType> map = {
        {"physical", CombatAttack::physical},
        {"qi", CombatAttack::qi},
        {"mana", CombatAttack::mana},
        {"psychic", CombatAttack::psychic}
    };

    if (auto it = map.find(s); it != map.end())
        return it->second;
    return std::nullopt;
}

std::optional<CombatAttack::AttackRange> CombatAttack::attackRangeFromString(const std::string& s){
    static const std::unordered_map<std::string, CombatAttack::AttackRange> map = {
        {"melee", CombatAttack::melee},
        {"ranged", CombatAttack::ranged},
        {"either", CombatAttack::either}
    };

    if (auto it = map.find(s); it != map.end())
        return it->second;
    return std::nullopt;
}