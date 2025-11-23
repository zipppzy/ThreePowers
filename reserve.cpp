#include "reserve.h"
#include <algorithm>

Reserve::Reserve(std::string name, double maxValue)
    : name{name},
    maxValue{maxValue}
{}

Reserve::Reserve(toml::table reserveTable, Reserve* parentReserve) {
    this->name = reserveTable["name"].value_or("");
    this->maxValue = reserveTable["maxValue"].value_or(0);
    this->currentValue = reserveTable["currentValue"].value_or(0);
    this->regen = reserveTable["regen"].value_or(0);
    //TODO: deal wil hasSubReserves and hasSoftMax
    this->parentReserve = parentReserve;
}

// Reserve::Reserve(const Reserve& other){
//     this->parentReserve = other.parentReserve;
//     //prob not good to do it this way maybe change later
//     this->subReserves = other.subReserves;
//     this->name = other.name;
//     this->maxValue = other.getMaxValue();
//     this->currentValue = other.getCurrentValue();
//     this->hasSubReserves = other.hasSubReserves;
//     this->hasSoftMax = other.hasSoftMax;

// }

bool Reserve::add(double value){
    if(this->currentValue+value > this->maxValue){
        if(!this->hasSoftMax){
            this->currentValue = this->maxValue;
        }else{
            //handle softMax here
        }
        return false;
    }else{
        this->currentValue += value;
        return true;
    }
}

bool Reserve::addSub(std::string name, double value){
    auto it  = std::find_if(this->subReserves.begin(), this->subReserves.end(), [&name](const Reserve& reserve){return reserve.name ==name;});
    if(it == subReserves.end()){
        return false;
    }else{
        if(this->currentValue+value > this->maxValue){
            it->add(maxValue-currentValue);
            this->currentValue = this->maxValue;
            return false;
        }else{
            it->add(value);
            this->currentValue+=value;
            return true;
        }
    }
}

bool Reserve::subtract(double value){
    if(this->currentValue-value < 0){
        currentValue = 0;
        return false;
    }else{
        this->currentValue-=value;
        return true;
    }
}

bool Reserve::subtractSub(std::string name, double value){
    auto it  = std::find_if(this->subReserves.begin(), this->subReserves.end(), [&name](const Reserve& reserve){return reserve.name ==name;});
    if(it == subReserves.end()){
        return false;
    }else{
        this->subtract(value);
        return it->subtract(value);
    }
}

void Reserve::addRegen(double value){
    this->regen += value;
}

double Reserve::getRegen() const{
    return this->regen;
}
void Reserve::applyRegen(){
    this->add(this->regen);
}
