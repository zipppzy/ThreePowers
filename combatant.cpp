#include "combatant.h"
#include <random>

Combatant::Combatant(const Combatant& other)
    : name(other.name)
    , health(other.health)
    , maxHealth(other.maxHealth)
    , speed(other.speed)
{
    for(const auto& [attack, weight] : other.availableAttacks){
        this->availableAttacks.push_back({std::make_shared<CombatAttack>(*attack), weight});
    }
    this->pickNextAttack();
}

Combatant::Combatant(toml::table combatantTable){
    this->name = combatantTable["name"].value_or("");
    this->maxHealth = combatantTable["maxHealth"].value_or(0.0);
    this->health = this->maxHealth;
    this->speed = combatantTable["speed"].value_or(0.0);

    if(auto attacksArray = combatantTable["attacks"].as_array()){
        for(const auto& elem : *attacksArray){
            if(auto attackTable = elem.as_table()){
                std::string attackName = attackTable->at("name").value_or("");
                double weight = attackTable->at("weight").value_or(1.0);
                if(auto maybeAttack = CombatAttack::checkAttackDatabase(attackName)){
                    this->availableAttacks.push_back({std::make_shared<CombatAttack>(maybeAttack.value()), weight});
                }
            }
        }
    }

    this->pickNextAttack();
}

bool Combatant::tick(){
    if(!this->currentAttack) return false;
    return this->currentAttack->tick();
}

void Combatant::pickNextAttack(){
    if(this->availableAttacks.empty()){
        this->currentAttack = nullptr;
        return;
    }

    std::vector<double> weights;
    for(const auto& [attack, weight] : this->availableAttacks){
        weights.push_back(weight);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<int> dist(weights.begin(), weights.end());

    int chosenIndex = dist(gen);
    this->currentAttack = std::make_shared<CombatAttack>(*this->availableAttacks[chosenIndex].first);
}

void Combatant::takeDamage(double amount){
    this->health -= amount;
    if(this->health < 0){
        this->health = 0;
    }
}

bool Combatant::isDead() const{
    return this->health <= 0;
}

std::unordered_map<std::string, Combatant> Combatant::combatantDatabase;

void Combatant::loadCombatantDatabase(const std::string& path){
    auto in = toml::parse_file(path);

    Combatant::combatantDatabase.clear();
    if(auto combatantsArray = in["combatants"].as_array()){
        for(auto& elem : *combatantsArray){
            if(auto combatantTable = elem.as_table()){
                Combatant::combatantDatabase.insert_or_assign(combatantTable->at("name").value_or(""), Combatant(*combatantTable));
            }
        }
    }
}

std::optional<std::reference_wrapper<const Combatant>> Combatant::checkCombatantDatabase(const std::string& name){
    if(auto search = Combatant::combatantDatabase.find(name); search != Combatant::combatantDatabase.end()){
        return std::cref(search->second);
    }else{
        return std::nullopt;
    }
}