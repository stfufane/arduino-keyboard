#include "DigitDisplay.h"
#include <stdio.h>

DigitDisplay::DigitDisplay()
    : mBuffer {' ', ' ', ' ', ' '}
{
    // Init the character conversion table with zeros
    for (int i = 0; i < 128; i++) {
        mCharConversion[i] = 0b00000000;
    }
    // Then define the values we will use
    /*  Segments position
              A
            F   B
              G
            E   C
              D   dp
    */
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
    mCharConversion['c'] = 0b00011001; 
    mCharConversion['d'] = 0b00111011; 
    mCharConversion['e'] = 0b11011001; 
    mCharConversion['f'] = 0b11010001;
    mCharConversion['i'] = 0b00010000;
    mCharConversion['n'] = 0b00010011;
    mCharConversion['o'] = 0b00011011;
    mCharConversion['p'] = 0b11110001;
    mCharConversion['r'] = 0b00010001;
    mCharConversion['t'] = 0b01011001;
    mCharConversion['A'] = 0b11110011;
    mCharConversion['B'] = 0b11111011;
    mCharConversion['C'] = 0b11011000;
    mCharConversion['G'] = 0b11011010;
    mCharConversion['H'] = 0b01110011;
    mCharConversion['I'] = 0b01010000;
    mCharConversion['L'] = 0b01011000;
    mCharConversion['O'] = 0b11111010;
    mCharConversion['S'] = 0b11001011; 
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
    mLastDisplay = micros();
    mLastUpdate = millis();
}

void DigitDisplay::setBuffer(const char prefix, const int value)
{
    sprintf(mBuffer, " %3d", value);
    mBuffer[0] = prefix;
    mLastUpdate = millis();
}

void DigitDisplay::setBuffer(const char* sequence)
{
    for (int c = 0; c < mNbDigits; c++)
    {
        mBuffer[c] = sequence[c];
    }
    mLastUpdate = millis();
}

void DigitDisplay::displayBuffer()
{
    if (micros() - mLastDisplay > 4000) 
    {
        mLastDisplay = micros();
        mLastDigit = mCurrentDigit;
        mCurrentDigit = (mCurrentDigit + 1) % mNbDigits;
        writeDigit();
    }
    if (millis() - mLastUpdate > 5000) sprintf(mBuffer, "    ");
}

void DigitDisplay::writeDigit()
{
    digitalWrite(mLatchPin, LOW);
    shiftOut(mDataPin, mClockPin, LSBFIRST, mCharConversion[mBuffer[mCurrentDigit]]);

    digitalWrite(mFirstDigitPin + mLastDigit, LOW); 
    digitalWrite(mLatchPin, HIGH);    
    digitalWrite(mFirstDigitPin + mCurrentDigit, HIGH); 
}

void DigitDisplay::displaySequenceBlocking(const char* sequence, int time)
{
    unsigned long now = millis(), until = millis() + time;
    setBuffer(sequence);
    while (now < until) {
        displayBuffer();
        now = millis();
    }
}
