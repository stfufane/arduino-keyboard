#include "Keyboard.h"
#include "Button.h"
#include "Modulation.h"
#include "PitchWheel.h"

Keyboard::Keyboard(midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> &midiInterface)
    : mMidiInterface(midiInterface), 
      mInputPins { 2, 3, 4, 5, 6, 7, 8, 9 }, 
      mOutputPins { 22, 23, 24, 25, 26, 27, 28, 29 }
{
    Button minus_octave(30, *this, &KeyboardCallbacks::lowerOctave);
    Button plus_octave(32, *this, &KeyboardCallbacks::upperOctave);

    Modulation modWheel(A0, 1, midiInterface);
    PitchWheel pitchWheel(A1, midiInterface);
}

void Keyboard::setup()
{
    // Declare column pins
    for (byte pin = 0; pin < sizeof(mOutputPins); pin++)
    {
        pinMode(mOutputPins[pin], OUTPUT);
    }

    // Declare row pins
    for (byte pin = 0; pin < sizeof(mInputPins); pin++)
    {
        pinMode(mInputPins[pin], INPUT_PULLUP);
    }

    // Init key map.
    int note = 36;
    for (int rowCtr = 1; rowCtr < NUM_ROWS; rowCtr += 2)
    {
        for (int colCtr = 0; colCtr < NUM_COLS; ++colCtr)
        {
            mKeyPressed[rowCtr][colCtr] = false;
            keyTime[rowCtr][colCtr] = 0;
            mKeyToMidiMap[rowCtr][colCtr] = note;
            note++;
        }
    }
}

void Keyboard::checkValues()
{
    // Loop on columns
    for (int colCtr = 0; colCtr < NUM_COLS; ++colCtr)
    {
        // Set column HIGH
        digitalWrite(mOutputPins[colCtr], HIGH);

        // Read all the rows for that column
        for (int row = 0; row < NUM_ROWS; row++)
        {
            mRowValue[row] = digitalRead(mInputPins[row]);
        }

        // Set column LOW
        digitalWrite(mOutputPins[colCtr], LOW);

        // process keys pressed
        // Start by odd rows cause they're hit first.
        for (int rowCtr = 1; rowCtr < NUM_ROWS; rowCtr += 2)
        {
            // If we detect a hit we record the time
            if (mRowValue[rowCtr] != 0 && !mKeyPressed[rowCtr][colCtr])
            {
                mKeyPressed[rowCtr][colCtr] = true;
                keyTime[rowCtr][colCtr] = micros();
            }

            // Then read the even rows and measure the time difference with the odd row.
            if (mRowValue[rowCtr - 1] != 0 && keyTime[rowCtr][colCtr] > 0)
            {
                keyTime[rowCtr][colCtr] = micros() - keyTime[rowCtr][colCtr];
                noteOn(rowCtr, colCtr);
                keyTime[rowCtr][colCtr] = 0;
            }
        }

        // process keys released
        for (int rowCtr = 1; rowCtr < NUM_ROWS; rowCtr += 2)
        {
            // If it's back to 0 then it's off.
            if (mRowValue[rowCtr] == 0 && mKeyPressed[rowCtr][colCtr])
            {
                mKeyPressed[rowCtr][colCtr] = false;
                mMidiInterface.sendNoteOff(mKeyToMidiMap[rowCtr][colCtr] + (mOctaveOffset * 12), 0, 1);
            }
        }
    }
}

void Keyboard::noteOn(int row, int col)
{
    unsigned long t = keyTime[row][col];
    if (t > MAX_TIME)
        t = MAX_TIME;
    if (t < MIN_TIME)
        t = MIN_TIME;

    // Manage a velocity curve.
    unsigned long velocity = MAX_VELOCITY - ((t - MIN_TIME) * MAX_VELOCITY / DIFF_TIME);
    byte vel = velocity;
    if (velocity < 127)
        vel = (((velocity * velocity) >> 7) * velocity) >> 7;

    // And send the note !
    mMidiInterface.sendNoteOn(mKeyToMidiMap[row][col] + (mOctaveOffset * 12), vel, 1);
}

