#pragma once

#include <Arduino.h>
#include "Debug.h"
#include "ControlPin.h"
#include "Keyboard.h"

class Button: public ControlPin
{
    public:
        Button(int pin, Keyboard &keyboard, KeyboardCallback keyboardCallback);
        void setup() override;
        void checkValue() override;
    private:
        int mState;
        int mLastState;
        KeyboardCallback mKeyboardCallback;
};
