#pragma once

#include <Arduino.h>
#include "Debug.h"

// Use of a 4 x 7 segments display 
class DigitDisplay
{
    public:
        DigitDisplay();
        void setup();
        void displaySequenceBlocking(const char* sequence, int time);
        void setBuffer(const char prefix, const int value);
        void setBuffer(const char* sequence);
        void displayBuffer();
    private:
        void writeDigit();
        const int mNbDigits = 4;
        const int mFirstDigitPin = 46;
        const int mClockPin = 42;
        const int mLatchPin = 43;
        const int mDataPin = 44;

        byte mCharConversion[128];
        char mBuffer[4];
        int mCurrentDigit;
        int mLastDigit;
        unsigned long mLastDisplay;
        unsigned long mLastUpdate;
};
