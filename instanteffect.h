#ifndef INSTANTEFFECT_H
#define INSTANTEFFECT_H

#include "toml.hpp"
#include <string>
#include <unordered_map>
#include <optional>


class InstantEffect {
public:
    enum EffectType {
        Log,
        Event,
        UnlockAction,
        UnlockLocation,
        AddItem,
        AddSkillXp,
        ModifyReserve
    };

    EffectType type;
    std::unordered_map<std::string, std::string> parameters;

    InstantEffect() = default;
    explicit InstantEffect(const toml::table& effectTable);

private:
    static std::optional<EffectType> effectTypeFromString(const std::string& s);
};

#endif // INSTANTEFFECT_H
