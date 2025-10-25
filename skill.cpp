#include "skill.h"

Skill::Skill(){};

Skill::Skill(std::string name, double baseXpForLevel, double scalingFactor, unsigned int maxLevel){
    this->name = name;
    this->baseXpForLevel = baseXpForLevel;
    this->scalingFactor = scalingFactor;
    this->maxLevel = maxLevel;
    this->level = 0;
    this->xp = 0;
}

Skill::Skill(std::string name, double baseXpForLevel, double scalingFactor, unsigned int level, double xp, unsigned int maxLevel){
    this->name = name;
    this->baseXpForLevel = baseXpForLevel;
    this->scalingFactor = scalingFactor;
    this->level = level;
    this->xp = xp;
    this->maxLevel = maxLevel;
}

Skill::Skill(toml::table skillTable){
    this->name = skillTable["name"].value_or("");
    this->level = skillTable["level"].value_or(0);
    this->xp = skillTable["xp"].value_or(0);
    this->baseXpForLevel = skillTable["baseXpForLevel"].value_or(-1.0);
    this->scalingFactor = skillTable["scalingFactor"].value_or(-1.0);
    this->maxLevel = skillTable["maxLevel"].value_or(0);

    if(auto effectsArray = skillTable["effects"].as_array()){
        for(auto&& effectNode : *effectsArray){
            if(auto table = effectNode.as_table()){
                Effect effect{*table};
                this->effects[effect.name].push_back(std::move(effect));
            }
        }
    }
}

std::unordered_map<std::string, Skill> Skill::skillDatabase;

unsigned int Skill::getLevel() const{
    return this->level;
}

double Skill::getXp() const{
    return xp;
}

double Skill::getXpToNextLevel() const{
    if(this->level >= this->maxLevel) return 0;
    return this->baseXpForLevel * pow(this->scalingFactor, this->level);
}

void Skill::addXp(double xp){
    if(level >= maxLevel) return;

    this->xp += xp;
    while (level < maxLevel) {
        double xpToLevel = baseXpForLevel * std::pow(scalingFactor, level);
        if (this->xp >= xpToLevel) {
            this->xp -= xpToLevel;
            level++;
        } else {
            break;
        }
    }

    if(level >= maxLevel){
        this->xp = 0;
    }
}

unsigned int Skill::getMaxLevel() const{
    return this->maxLevel;
}
toml::table Skill::getSkillTable() const{
    toml::table skill;
    skill.insert("name", name);
    skill.insert("level", level);
    skill.insert("xp", xp);
    skill.insert("baseXpForLevel", baseXpForLevel);
    skill.insert("scalingFactor", scalingFactor);
    skill.insert("maxLevel", maxLevel);
    return skill;
}

std::optional<std::reference_wrapper<const std::vector<Effect>>> Skill::checkEffects(const std::string& name) const{
    if (auto search = this->effects.find(name); search != this->effects.end()) {
        return std::cref(search->second);
    }
    return std::nullopt;
}

void Skill::loadSkillDatabase(std::string path){

    auto in = toml::parse_file(path);

    Skill::skillDatabase.clear();
    if(auto skillsArray = in["skills"].as_array(); skillsArray){
        for(auto& elem : *skillsArray){
            if(auto skillTable = elem.as_table()){
                Skill::skillDatabase[skillTable->at("name").value_or("")] = Skill(*skillTable);
            }
        }
    }
}

std::optional<std::reference_wrapper<const Skill>> Skill::checkSkillDatabase(std::string name){
    if(auto search = Skill::skillDatabase.find(name); search != Skill::skillDatabase.end()){
        return std::cref(search->second);
    }else{
        return std::nullopt;
    }
}
