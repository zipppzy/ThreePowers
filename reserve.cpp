#include "reserve.h"
#include <algorithm>

Reserve::Reserve(std::string name){
    this->name = name;
}

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
