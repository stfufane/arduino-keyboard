#pragma once

#include <Arduino.h>
// #include <vector>
#include <MIDI.h>

// Number of rows and columns of the keyboard scan matrix
#define NUM_ROWS 8
#define NUM_COLS 8

#define MAX_VELOCITY 127

#define MIN_TIME 2048
#define MAX_TIME 131072
#define DIFF_TIME (MAX_TIME - MIN_TIME)

class Keyboard
{
    public:
        Keyboard(midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> &midiInterface);
        void setup();
        void checkValues();

        void noteOn(int row, int col);
        int getOctaveOffset() { return mOctaveOffset; }
        void setOctaveOffset(int offset) { mOctaveOffset = offset; }
    private:
        midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> &mMidiInterface;
        // std::vector<ControlPin> mControlPins;

        // Row input pins
        byte mInputPins[NUM_ROWS];

        // Column output pins
        byte mOutputPins[NUM_COLS];

        // Key states and time between 2 signals
        bool mKeyPressed[NUM_ROWS][NUM_COLS];
        unsigned long keyTime[NUM_ROWS][NUM_COLS];
        uint8_t mKeyToMidiMap[NUM_ROWS][NUM_COLS];
        int mRowValue[NUM_ROWS];

        int mOctaveOffset = 0;
};

class KeyboardCallbacks
{
public:
    static void lowerOctave(Keyboard &keyboard)
    {
        if (keyboard.getOctaveOffset() > -3)
        {
            keyboard.setOctaveOffset(keyboard.getOctaveOffset() - 1);
        }
    }

    static void upperOctave(Keyboard &keyboard)
    {
        if (keyboard.getOctaveOffset() < 3)
        {
            keyboard.setOctaveOffset(keyboard.getOctaveOffset() + 1);
        }
    }
};

typedef void (*KeyboardCallback)(Keyboard &keyboard);