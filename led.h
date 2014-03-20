#ifndef LED_H
#define LED_H

#define RED 0
#define BLUE 1
#define GREEN 2

#include <string>

class Led{
public:
    Led();
    static bool setRate(int*);
    static bool setColor(int*);
    static bool setState(int*);
    static bool getRate(int*);
    static bool getColor(int*);
    static bool getState(int*);
private:
    int _rate;
    int _color;
    bool _state;
};

#endif
