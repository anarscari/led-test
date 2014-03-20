#include "led.h"

#include <stdio.h>
#include <assert.h>

static Led *led = 0;
Led::Led(){
    _rate = 0;
    _color = RED;
    _state = 0;
    led = this;
}

bool Led::setRate(int *arg){
    assert(led);
    led->_rate = *arg;
    return true;
}
bool Led::setColor(int *arg){
    assert(led);
    led->_color = *arg;
    return true;
}
bool Led::setState(int *arg){
    assert(led);
    led->_state = *arg;
    return true;
}
bool Led::getRate(int *res){
    assert(led);
    *res = led->_rate;
    return true;
}
bool Led::getColor(int *res){
    assert(led);
    *res = led->_color;
    return true;
}
bool Led::getState(int *res){
    assert(led);
    *res = led->_state;
    return true;
}
