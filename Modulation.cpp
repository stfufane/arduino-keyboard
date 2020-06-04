#include "Modulation.h"

Modulation::Modulation(int pin, int cc, Keyboard &keyboard)
    : ControlPin(pin, keyboard),
      mCc(cc)
{
    pinMode(mPin, INPUT);
}

void Modulation::setup()
{
    mValue = analogRead(mPin);
    mLastValue = mValue;
    int mLastSentValue = 0;
    mLastSentTime = millis();
}

void Modulation::checkValue()
{
    if (millis() - mLastSentTime < 5)
        return;
    
    mValue = analogRead(mPin);
    if (abs(mLastValue - mValue) < 8)
        return;
    
    int value = map(mValue, 0, 1023, 0, 127);
    if (value != mLastSentValue)
    {
        mKeyboard.getMidi().sendControlChange(mCc, value, 1);
        // mKeyboard.getDisplay()->setBuffer('c', value);
        DBG("CC");
        DBG(value);
        mLastValue = mValue;
        mLastSentValue = value;
        mLastSentTime = millis();
    }
}
