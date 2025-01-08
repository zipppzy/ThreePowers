#include "player.h"

Player::Player() {
};

bool Player::hasSkill(std::string name){
    for(Skill i : this->skills){
        if(i.name == name){
            return true;
        }
    }
    return false;
};

void Player::savePlayerState(){
    toml::array playerData;

    toml::table attributes;
    attributes.insert("age", std::to_string(age));              //stored as string to avoid length limit
    attributes.insert("lifespan", std::to_string(lifespan));    //stored as string to avoid length limit
    attributes.insert("qi", maxQi);
    attributes.insert("maxQi", maxQi);

    playerData.push_back(std::move(attributes));

    toml::array skills;
    for(Skill i : this->skills){
        skills.push_back(i.getSkillTable());
    }

    playerData.push_back(std::move(skills));
}
