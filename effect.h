#ifndef EFFECT_H
#define EFFECT_H
//defines an effects skills can have
#include <string>
struct Effect{
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
};

#endif // EFFECT_H
