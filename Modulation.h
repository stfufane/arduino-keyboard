#pragma once

#include <Arduino.h>
#include <MIDI.h>
#include "Debug.h"
#include "ControlPin.h"

class Modulation : public ControlPin
{
    public:
        Modulation(byte pin, int cc, midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> &midiInterface);
        void setup() override;
        // Read value at each loop
        void checkValue() override;
    private:
        // Reference to the midi interface declared in main sketch
        midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> &mMidiInterface;
        int mCc;
        // Potentiometer value
        int mValue;
        int mLastValue;
        // MIDI value
        int mLastSentValue;
        unsigned long mLastSentTime; // Time of last signal sent
};
