#pragma once

#include <Arduino.h>
#include <MIDI.h>
#include "Debug.h"
#include "ControlPin.h"

class PitchWheel : public ControlPin
{
    public:
        PitchWheel(byte pin, midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> &midiInterface);
        void setup() override;
        // Read value at each loop
        void checkValue() override;
    private:
        // Reference to the midi interface declared in main sketch
        midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> &mMidiInterface;
        
        // Potentiometer value
        int mValue;
        int mLastValue;
        // Potentiometer value when it comes back to its original position.
        int mCenter;
        // MIDI value
        int mLastSentValue;
        unsigned long mLastSentTime; // Time of last signal sent

        void sendMessage(int value);
};