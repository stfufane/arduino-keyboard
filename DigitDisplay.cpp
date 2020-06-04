#include "DigitDisplay.h"
#include <stdio.h>

DigitDisplay::DigitDisplay()
    : mBuffer {' ', ' ', ' ', ' '}
{
    /*  Segments position
              A
            F   B
              G
            E   C
              D   dp
    */

    // Init the character conversion table with zeros
    for (int i = 0; i < 128; i++) {
        mCharConversion[i] = 0b00000000;
    }
    // Then define the values we will use
    // The segment bits are ordered like this : A F B E D DP C G
    mCharConversion[' '] = 0b00000000;
    mCharConversion['0'] = 0b11111010;
    mCharConversion['1'] = 0b00100010;
    mCharConversion['2'] = 0b10111001; 
    mCharConversion['3'] = 0b10101011; 
    mCharConversion['4'] = 0b01100011; 
    mCharConversion['5'] = 0b11001011; 
    mCharConversion['6'] = 0b11011011;
    mCharConversion['7'] = 0b10100010; 
    mCharConversion['8'] = 0b11111011; 
    mCharConversion['9'] = 0b11101011; 
    mCharConversion['a'] = 0b11110011;
    mCharConversion['b'] = 0b01011011; 
    mCharConversion['c'] = 0b11011000; 
    mCharConversion['d'] = 0b00111011; 
    mCharConversion['e'] = 0b11011001; 
    mCharConversion['f'] = 0b11010001; 
    mCharConversion['p'] = 0b11110001;
    mCharConversion['-'] = 0b00000001;
}

void DigitDisplay::setup()
{
    pinMode(mDataPin, OUTPUT);
    pinMode(mClockPin, OUTPUT);
    pinMode(mLatchPin, OUTPUT);
    for (int digit = 0; digit < mNbDigits; digit ++) {
        pinMode(mFirstDigitPin + digit, OUTPUT);
    }
    mCurrentDigit = 0;
    mLastDisplay = millis();
}

void DigitDisplay::setBuffer(const char prefix, const int value)
{
    sprintf(mBuffer, " %3d", value);
    mBuffer[0] = prefix;
}

void DigitDisplay::displayBuffer()
{
    if (millis() - mLastDisplay > 4) 
    {
        mLastDigit = mCurrentDigit;
        mCurrentDigit = (mCurrentDigit + 1) % mNbDigits;
        writeDigit();
    }
}

void DigitDisplay::writeDigit()
{
    digitalWrite(mLatchPin, LOW);
    shiftOut(mDataPin, mClockPin, LSBFIRST, mCharConversion[mBuffer[mCurrentDigit]]);

    digitalWrite(mFirstDigitPin + mLastDigit, LOW); 
    digitalWrite(mLatchPin, HIGH);    
    digitalWrite(mFirstDigitPin + mCurrentDigit, HIGH); 
}
