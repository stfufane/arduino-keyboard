#include "Modulation.h"

Modulation::Modulation(byte pin, int cc, midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> &midiInterface)
    : mMidiInterface(midiInterface),
      ControlPin(pin),
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
        mMidiInterface.sendControlChange(mCc, value, 1);
        DBG("CC");
        mLastValue = mValue;
        mLastSentValue = value;
        mLastSentTime = millis();
    }
}
