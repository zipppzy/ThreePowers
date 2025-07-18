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
}

std::unordered_map<std::string, Skill> Skill::skillDatabase;

unsigned int Skill::getLevel() const{
    return this->level;
}

void Skill::addXp(double xp){
    if(level >= maxLevel) return;

    this->xp += xp;
    double xpToLevel = this->baseXpForLevel*std::pow(this->scalingFactor, this->level);
    if(this->xp > xpToLevel){
        this->xp -= xpToLevel;
        this->level += 1;
    }

    if(level >= maxLevel){
        this->xp = 0;
    }
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
