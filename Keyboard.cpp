#include "Keyboard.h"
#include "LED.h"
#include "Button.h"
#include "Modulation.h"
#include "PitchWheel.h"

Keyboard::Keyboard(midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> &midiInterface)
    : mMidiInterface(midiInterface), 
      mInputPins { 2, 3, 4, 5, 6, 7, 8, 9 }, 
      mOutputPins { 22, 23, 24, 25, 26, 27, 28, 29 },
      mDigitDisplay(),
      mControlPins { 
                     new Button(30, *this, &KeyboardCallbacks::transposeDown),
                     new Button(32, *this, &KeyboardCallbacks::transposeUp),
                     new Button(34, *this, &KeyboardCallbacks::toggleHold, new LED(36, false)),
                     new Button(38, *this, KeyboardCallbacks::switchTransposeMode),
                     new Modulation(A0, 1, *this),
                     new PitchWheel(A1, *this)
                   }
{  
    // Declare column pins of the scan matrix
    for (int pin = 0; pin < NUM_COLS; pin++)
    {
        pinMode(mOutputPins[pin], OUTPUT);
    }

    // Declare row pins of the scan matrix
    for (int pin = 0; pin < NUM_ROWS; pin++)
    {
        pinMode(mInputPins[pin], INPUT_PULLUP);
    }
}

void Keyboard::setup()
{
    // Init display pins.
    mDigitDisplay.setup();

    // Make pitch bend modifiable.
    sendSysEx(0x0c, 0x01);

    // Setup control pins
    for (int pin = 0; pin < NB_PINS; pin++)
    {
        mControlPins[pin]->setup();
    }

    // Init key map.
    int note = 36;
    for (int rowCtr = 1; rowCtr < NUM_ROWS; rowCtr += 2)
    {
        for (int colCtr = 0; colCtr < NUM_COLS; ++colCtr)
        {
            mKeyPressed[rowCtr][colCtr] = false;
            mKeyTime[rowCtr][colCtr] = 0;
            mKeyToMidiMap[rowCtr][colCtr] = note;
            note++;
        }
    }
    // Display a silly text sequence at startup
    mDigitDisplay.displaySequenceBlocking("COOL", 1000);
    mDigitDisplay.displaySequenceBlocking("BOOG", 1000);
    mDigitDisplay.setBuffer("    ");
}

void Keyboard::checkValues()
{
    // Read control pins
    for (int pin = 0; pin < NB_PINS; pin++)
    {
        mControlPins[pin]->checkValue();
    }

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
                mKeyTime[rowCtr][colCtr] = micros();
            }

            // Then read the even rows and measure the time difference with the odd row.
            if (mRowValue[rowCtr - 1] != 0 && mKeyTime[rowCtr][colCtr] > 0)
            {
                mKeyTime[rowCtr][colCtr] = micros() - mKeyTime[rowCtr][colCtr];
                noteOn(rowCtr, colCtr);
                mKeyTime[rowCtr][colCtr] = 0;
            }
        }

        // process keys released
        for (int rowCtr = 1; rowCtr < NUM_ROWS; rowCtr += 2)
        {
            // If it's back to 0 then it's off.
            if (mRowValue[rowCtr] == 0 && mKeyPressed[rowCtr][colCtr])
            {
                mKeyPressed[rowCtr][colCtr] = false;
                if (!mHold) 
                    mMidiInterface.sendNoteOff(mKeyToMidiMap[rowCtr][colCtr] + (mOctaveOffset * 12) + mSemitoneOffset, 0, 1);
                DBG("Note off");
            }
        }
    }
    mDigitDisplay.displayBuffer();
}

void Keyboard::toggleHold()
{ 
    if (mHold && mHeldNote != -1) {
        mMidiInterface.sendNoteOff(mHeldNote, 0, 1);
        mHeldNote = -1;
    }
    mHold = !mHold;
}

void Keyboard::noteOn(int row, int col)
{
    DBG("Note on");
    unsigned long t = mKeyTime[row][col];
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
    int note = mKeyToMidiMap[row][col] + (mOctaveOffset * 12) + mSemitoneOffset;
    mMidiInterface.sendNoteOn(note, vel, 1);
    if (mHold) {
        if (mHeldNote > -1)
           mMidiInterface.sendNoteOff(mHeldNote, 0, 1);
        mHeldNote = note;
    }
    mDigitDisplay.setBuffer('n', note);
}

void Keyboard::sendSysEx(byte idx7, byte idx9)
{
    mSysexArray[7] = idx7;
    mSysexArray[9] = idx9;
    mMidiInterface.sendSysEx(11, mSysexArray, true);
}

/**
 * Callbacks that will be called by buttons (see Button class)
 */

void KeyboardCallbacks::switchTransposeMode(Keyboard &keyboard)
{
    keyboard.setTransposeMode((keyboard.getTransposeMode() + 1) % TRANSPOSE_MODES);
    keyboard.getDisplay()->setBuffer(keyboard.getTransposeModeText());
}

void KeyboardCallbacks::transposeDown(Keyboard &keyboard)
{
    switch (keyboard.getTransposeMode())
    {
        // 0 is octave transposition (from -3 to 3)
        case 0:
            if (keyboard.getOctaveOffset() > -3)
            {
                keyboard.setOctaveOffset(keyboard.getOctaveOffset() - 1);
                keyboard.displayTransposition(keyboard.getOctaveOffset());
            }
            break;
        // 1 is semitone transposition from -12 to 12
        case 1:
            if (keyboard.getSemitoneOffset() > -12)
            {
                keyboard.setSemitoneOffset(keyboard.getSemitoneOffset() - 1);
                keyboard.displayTransposition(keyboard.getSemitoneOffset());
            }
            break;
        // 2 is pitch bend range from 0 to 12
        case 2:
            if (keyboard.getPitchBendRange() > 0)
            {
                keyboard.setPitchBendRange(keyboard.getPitchBendRange() - 1);
                keyboard.sendSysEx(0x03, keyboard.getPitchBendRange());
                keyboard.displayTransposition(keyboard.getPitchBendRange());
            }
            break;
        // 3 is key priority from 0 to 2
        case 3:
            if (keyboard.getKeyPriority() > 0)
            {
                keyboard.setKeyPriority(keyboard.getKeyPriority() - 1);
                keyboard.sendSysEx(0x01, keyboard.getKeyPriority());
                keyboard.getDisplay()->setBuffer(keyboard.getKeyPriorityText());
            }
            break;
    }
}

void KeyboardCallbacks::transposeUp(Keyboard &keyboard)
{
    switch (keyboard.getTransposeMode())
    {
        // 0 is octave transposition (from -3 to 3)
        case 0:
            if (keyboard.getOctaveOffset() < 3)
            {
                keyboard.setOctaveOffset(keyboard.getOctaveOffset() + 1);
                keyboard.displayTransposition(keyboard.getOctaveOffset());
            }
            break;
        // 1 is semitone transposition from -12 to 12
        case 1:
            if (keyboard.getSemitoneOffset() < 12)
            {
                keyboard.setSemitoneOffset(keyboard.getSemitoneOffset() + 1);
                keyboard.displayTransposition(keyboard.getSemitoneOffset());
            }
            break;
        // 2 is pitch bend range from 0 to 12
        case 2:
            if (keyboard.getPitchBendRange() < 12)
            {
                keyboard.setPitchBendRange(keyboard.getPitchBendRange() + 1);
                keyboard.sendSysEx(0x03, keyboard.getPitchBendRange());
                keyboard.displayTransposition(keyboard.getPitchBendRange());
            }
            break;
        // 3 is key priority from 0 to 2
        case 3:
            if (keyboard.getKeyPriority() < 2)
            {
                keyboard.setKeyPriority(keyboard.getKeyPriority() + 1);
                keyboard.sendSysEx(0x01, keyboard.getKeyPriority());
                keyboard.getDisplay()->setBuffer(keyboard.getKeyPriorityText());
            }
            break;
    }
}

void KeyboardCallbacks::toggleHold(Keyboard &keyboard)
{
    keyboard.toggleHold();
}
