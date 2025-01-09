#include "player.h"

Player::Player() {
}

Player::Player(unsigned long long int age, unsigned long long int lifespan, Location* currentLocation, Action* currentAction){
    this->age = age;
    this->lifespan = lifespan;
    this->currentLocation = currentLocation;
    this->currentAction = currentAction;
    this->qi = 0;
    this->maxQi = 100;
    this->skills.push_back(Skill("Fireball", 100, 1.1));
}

bool Player::hasSkill(std::string name){
    for(Skill i : this->skills){
        if(i.name == name){
            return true;
        }
    }
    return false;
}

void Player::savePlayerState(){
    toml::table playerData;

    toml::table attributes;
    attributes.insert("age", std::to_string(age));              //stored as string to avoid length limit
    attributes.insert("lifespan", std::to_string(lifespan));    //stored as string to avoid length limit
    attributes.insert("qi", qi);
    attributes.insert("maxQi", maxQi);

    playerData.insert("attributes", std::move(attributes));

    toml::array skills;
    for(Skill i : this->skills){
        skills.push_back(i.getSkillTable());
    }

    playerData.insert("skills", std::move(skills));

    std::ofstream file("player_state.toml");
    file << playerData;
    file.close();
}

void Player::loadPlayerState(){
    auto playerData = toml::parse_file("player_state.toml");

    auto attributes = playerData["attributes"];
    this->age = std::stoull(playerData["age"].value_or("0"));
    this->lifespan = std::stoull(playerData["lifespan"].value_or("0"));
    this->qi = attributes["qi"].value_or(0.0);
    this->maxQi = attributes["maxQi"].value_or(0.0);

    this->skills.clear();  // Clear existing skills before loading
    if(auto skillsArray = playerData["skills"].as_array()){
        for(const auto& elem : *skillsArray){
            if(auto skillTable = elem.as_table()){
                this->skills.push_back(Skill(*skillTable));
            }
        }
    }
}
