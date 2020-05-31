#include "Button.h"

Button::Button(int pin, Keyboard &keyboard, KeyboardCallback keyboardCallback)
    : ControlPin(pin), 
      mKeyboard(keyboard),
      mKeyboardCallback(keyboardCallback)
{}

void Button::setup()
{
    pinMode(mPin, INPUT);
}

void Button::checkValue()
{
    mState = digitalRead(mPin);
    if (mState != mLastState && mState == HIGH) {
        mKeyboardCallback(mKeyboard);
    }
    mLastState = mState;
}
