#pragma once

#include <Arduino.h>
#include <MIDI.h>
#include "Debug.h"
#include "Keyboard.h"
#include "ControlPin.h"
#include "DigitDisplay.h"

class Modulation : public ControlPin
{
    public:
        Modulation(int pin, int cc, Keyboard& keyboard);
        void setup() override;
        // Read value at each loop
        void checkValue() override;
    private:
        int mCc;
        // Potentiometer value
        int mValue;
        int mLastValue;
        // MIDI value
        int mLastSentValue;
        unsigned long mLastSentTime; // Time of last signal sent
};
