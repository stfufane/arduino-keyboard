#pragma once

#include <MIDI.h>
#include <Arduino.h>
#include "Debug.h"
#include "DigitDisplay.h"

// Number of rows and columns of the keyboard scan matrix
#define NUM_ROWS 8
#define NUM_COLS 8

#define TRANSPOSE_MODES 4

// Number of control pins
#define NB_PINS 6

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

        void setTransposeMode(int transposeMode) { mTransposeMode = transposeMode; }
        int getTransposeMode() { return mTransposeMode; }
        const char* getTransposeModeText() { return mTransposeModes[mTransposeMode]; }
        const char* getKeyPriorityText() { return mKeyPriorities[mKeyPriority]; }
        
        void displayTransposition(int transposition) { mDigitDisplay.setBuffer((transposition < 0 ? '-' : ' '), abs(transposition)); }

        int getOctaveOffset() { return mOctaveOffset; }
        void setOctaveOffset(int offset) { mOctaveOffset = offset; }

        int getSemitoneOffset() { return mSemitoneOffset; }
        void setSemitoneOffset(int offset) { mSemitoneOffset = offset; }

        int getPitchBendRange() { return mPitchBendRange; }
        void setPitchBendRange(int range) { mPitchBendRange = range; }

        int getKeyPriority() { return mKeyPriority; }
        void setKeyPriority(int priority) { mKeyPriority = priority; }

        void toggleHold();
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

        // Strings to display when changing the transpose mode.
        const char *mTransposeModes[TRANSPOSE_MODES] = { "Octa", "tran", "bend", "prio" };
        const char *mKeyPriorities[3] = { "  LO", "HIGH", "LASt" };
        // Transpose button variables.
        int mTransposeMode = 0;
        int mOctaveOffset = 0;
        int mSemitoneOffset = 0;
        int mPitchBendRange = 2;
        int mKeyPriority = 2;

        bool mHold = false;
        int mHeldNote = -1;
        
        void noteOn(int row, int col);
};

class KeyboardCallbacks
{
public:
    static void switchTransposeMode(Keyboard &keyboard);
    static void transposeDown(Keyboard &keyboard);
    static void transposeUp(Keyboard &keyboard);
    static void toggleHold(Keyboard &keyboard);
};

typedef void (*KeyboardCallback)(Keyboard &keyboard);
