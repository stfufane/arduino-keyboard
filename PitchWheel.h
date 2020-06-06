#pragma once

#include <Arduino.h>
#include <MIDI.h>
#include "Debug.h"
#include "Keyboard.h"
#include "ControlPin.h"
#include "DigitDisplay.h"

class PitchWheel : public ControlPin
{
    public:
        PitchWheel(int pin, Keyboard& keyboard);
        void setup() override;
        // Read value at each loop
        void checkValue() override;
    private:        
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
