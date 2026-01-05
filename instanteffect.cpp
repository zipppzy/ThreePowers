#include "instanteffect.h"

InstantEffect::InstantEffect(const toml::table& effectTable) {
    std::string typeStr = effectTable["type"].value_or("");
    type = effectTypeFromString(typeStr).value_or(Log);

    // Load all parameters as strings
    if (auto paramsTable = effectTable["parameters"].as_table()) {
        for (const auto& [key, value] : *paramsTable) {
            if (auto str = value.value<std::string>()) {
                parameters[std::string(key)] = *str;
            } else if (auto num = value.value<double>()) {
                parameters[std::string(key)] = std::to_string(*num);
            } else if (auto num = value.value<int64_t>()) {
                parameters[std::string(key)] = std::to_string(*num);
            }
        }
    }
}

std::optional<InstantEffect::EffectType> InstantEffect::effectTypeFromString(const std::string& s) {
    static const std::unordered_map<std::string, EffectType> map = {
        {"log", Log},
        {"event", Event},
        {"unlock_action", UnlockAction},
        {"unlock_location", UnlockLocation},
        {"add_item", AddItem},
        {"add_skill_xp", AddSkillXp},
        {"modify_reserve", ModifyReserve}
    };

    if (auto it = map.find(s); it != map.end())
        return it->second;
    return std::nullopt;
}
