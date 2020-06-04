#pragma once

#include "Keyboard.h"

class ControlPin
{
    public:
        explicit ControlPin(int pin, Keyboard& keyboard) : mPin(pin), mKeyboard(keyboard) {};
        virtual void setup() = 0;
        virtual void checkValue() = 0;
    protected:
        int mPin;
        Keyboard& mKeyboard;
};
