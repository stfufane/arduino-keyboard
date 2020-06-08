#include "Button.h"

Button::Button(int pin, Keyboard &keyboard, KeyboardCallback keyboardCallback)
    : ControlPin(pin, keyboard), 
      mKeyboardCallback(keyboardCallback)
{
    pinMode(mPin, INPUT);
}

Button::Button(int pin, Keyboard &keyboard, KeyboardCallback keyboardCallback, LED* led)
    : ControlPin(pin, keyboard),
      mKeyboardCallback(keyboardCallback),
      mLed(led)
{
    pinMode(mPin, INPUT);
}

void Button::setup()
{
    mState = LOW;
    mLastState = LOW;
    if (mLed != nullptr) mLed->setup();
}

void Button::checkValue()
{
    mState = digitalRead(mPin);
    if (mState != mLastState && mState == HIGH) {
        mKeyboardCallback(mKeyboard);
        if (mLed != nullptr) mLed->toggleLight();
    }
    mLastState = mState;
}
