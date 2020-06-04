#pragma once

#include <MIDI.h>
#include <Arduino.h>
#include "Debug.h"
#include "DigitDisplay.h"

// Number of rows and columns of the keyboard scan matrix
#define NUM_ROWS 8
#define NUM_COLS 8

// Number of control pins
#define NB_PINS 4

#define MAX_VELOCITY 127

#define MIN_TIME 2048
#define MAX_TIME 131072
#define DIFF_TIME (MAX_TIME - MIN_TIME)

class ControlPin;

class Keyboard
{
    public:
        Keyboard(midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> &midiInterface);
        void setup();
        void checkValues();

        int getOctaveOffset() { return mOctaveOffset; }
        void setOctaveOffset(int offset) { mOctaveOffset = offset; }
        DigitDisplay* getDisplay() { return &mDigitDisplay; }
        midi::MidiInterface<midi::SerialMIDI<HardwareSerial>>& getMidi() { return mMidiInterface; }
    private:
        midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> &mMidiInterface;
        ControlPin* mControlPins[NB_PINS];

        DigitDisplay mDigitDisplay;

        // Row input pins of the scan matrix
        byte mInputPins[NUM_ROWS];
        // Column output pins of the scan matrix
        byte mOutputPins[NUM_COLS];

        // Key states and time between 2 signals
        bool mKeyPressed[NUM_ROWS][NUM_COLS];
        unsigned long mKeyTime[NUM_ROWS][NUM_COLS];
        uint8_t mKeyToMidiMap[NUM_ROWS][NUM_COLS];
        int mRowValue[NUM_ROWS];

        int mOctaveOffset = 0;
        
        void noteOn(int row, int col);
};

class KeyboardCallbacks
{
public:
    static void lowerOctave(Keyboard &keyboard);
    static void upperOctave(Keyboard &keyboard);
};

typedef void (*KeyboardCallback)(Keyboard &keyboard);
