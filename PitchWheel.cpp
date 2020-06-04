#include "PitchWheel.h"

PitchWheel::PitchWheel(int pin, Keyboard& keyboard)
    : ControlPin(pin, keyboard)
{
    pinMode(mPin, INPUT);
}

void PitchWheel::setup()
{
    // Init center value with current position, it will be the 0 pitch
    mCenter = analogRead(mPin);
    mValue = mCenter;
    mLastValue = mCenter;
    mLastSentValue = 0;
    mLastSentTime = millis();
}

void PitchWheel::checkValue()
{
    // To avoid sending too many values.
    if (millis() - mLastSentTime < 5)
        return;
    
    mValue = analogRead(mPin);
    
    // There's a dead zone at the center so we need to be sure we send 0 when we're very close.
    if (abs(mValue - mCenter) < 4 && mLastSentValue != 0) {
        sendMessage(0, 0);
        return;
    }

    // No need to send a message if the difference between values is not significant enough.
    if (abs(mLastValue - mValue) < 16) 
        return;
    
    int value, display;
    if (mValue < mCenter) {
        value = int(double(mCenter - mValue) / mCenter * double(MIDI_PITCHBEND_MIN));
        display = map(mValue, 0, mCenter, 0, 64);
    } else {
        value = int(double(mValue - mCenter) / double(1023.0f - mCenter) * double(MIDI_PITCHBEND_MAX));
        display = map(mValue, mCenter, 1023, 64, 127);
    }

    if (value != mLastSentValue)
    {
        sendMessage(value, display);
    }
}

void PitchWheel::sendMessage(int value, int display)
{
    DBG("Pitch");
    // mKeyboard.getDisplay()->setBuffer('p', display);
    mKeyboard.getMidi().sendPitchBend(value, 1);
    mLastValue = mValue;
    mLastSentValue = value;
    mLastSentTime = millis();
}
