#pragma once

#include <Arduino.h>
#include "Debug.h"
#include "ControlPin.h"
#include "LED.h"
#include "Keyboard.h"

class Button: public ControlPin
{
    public:
        Button(int pin, Keyboard &keyboard, KeyboardCallback keyboardCallback);
        Button(int pin, Keyboard &keyboard, KeyboardCallback keyboardCallback, LED* led);
        void setup() override;
        void checkValue() override;
    private:
        int mState;
        int mLastState;
        LED* mLed = nullptr;
        KeyboardCallback mKeyboardCallback;
};
