#include "effect.h"

Effect::Effect(const toml::table& effectTable){
    name = effectTable["name"].value_or("");

    std::string typeStr = effectTable["type"].value_or("");
    type = effectTypeFromString(typeStr).value_or(multDuration);
    magnitude = effectTable["magnitude"].value_or(0.0);
}

std::optional<Effect::EffectType> Effect::effectTypeFromString(const std::string& s){
    static const std::unordered_map<std::string, Effect::EffectType> map = {
        {"multDuration",      Effect::multDuration},
        {"flatDuration",      Effect::flatDuration},
        {"multFailureChance", Effect::multFailureChance},
        {"flatFailureChance", Effect::flatFailureChance},
        {"multMaxValue",      Effect::multMaxValue},
        {"flatMaxValue",      Effect::flatMaxValue}
    };

    if (auto it = map.find(s); it != map.end())
        return it->second;
    return std::nullopt;
}
