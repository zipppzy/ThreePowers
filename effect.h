#ifndef EFFECT_H
#define EFFECT_H

#include "toml.hpp"
#include <optional>
#include <string>
#include <unordered_map>

//defines an effects skills can have
class Effect{
public:
    enum EffectType{
        //Action effects
        multDuration,
        flatDuration,
        multFailureChance,
        flatFailureChance,
        //Reserve effects
        multMaxValue,
        flatMaxValue
    };

    std::string name;   //name of action or reserve
    EffectType type;
    double magnitude;

    //Default Constructor
    Effect() : name(""), type(multDuration), magnitude(0.0) {}
    Effect(const toml::table& effectTable);

    //lookup table for EffectType Enum
    static std::optional<Effect::EffectType> effectTypeFromString(const std::string& s);
};
#endif // EFFECT_H
