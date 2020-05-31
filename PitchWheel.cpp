#include "PitchWheel.h"

PitchWheel::PitchWheel(byte pin, midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> &midiInterface)
    : mMidiInterface(midiInterface), 
      ControlPin(pin)
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
        sendMessage(0);
        return;
    }

    // No need to send a message if the difference between values is not significant enough.
    if (abs(mLastValue - mValue) < 4) 
        return;
    
    int value;
    if (mValue < mCenter) {
        value = int(double(mCenter - mValue) / mCenter * double(MIDI_PITCHBEND_MIN));
    } else {
        value = int(double(mValue - mCenter) / double(1023.0f - mCenter) * double(MIDI_PITCHBEND_MAX));
    }

    if (value != mLastSentValue)
    {
        sendMessage(value);
    }
}

void PitchWheel::sendMessage(int value)
{
    mMidiInterface.sendPitchBend(value, 1);
    mLastValue = mValue;
    mLastSentValue = value;
    mLastSentTime = millis();
}