#include "Button.h"

Button::Button(int pin, Keyboard &keyboard, KeyboardCallback keyboardCallback)
    : ControlPin(pin), 
      mKeyboard(keyboard),
      mKeyboardCallback(keyboardCallback)
{
    pinMode(mPin, INPUT);
}

void Button::setup()
{
    mState = LOW;
    mLastState = LOW;
}

void Button::checkValue()
{
    mState = digitalRead(mPin);
    if (mState != mLastState && mState == HIGH) {
        mKeyboardCallback(mKeyboard);
    }
    mLastState = mState;
}
