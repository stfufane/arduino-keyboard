#include "MIDI_Keyboard.h"

void initKeys()
{
    minus_octave = { MINUS_OCTAVE, LOW, LOW, lowerOctave };
    plus_octave = { PLUS_OCTAVE, LOW, LOW, upperOctave };

    // Init key map.
    int note = 36;
    for (int rowCtr = 1; rowCtr < NUM_ROWS; rowCtr += 2)
    {
        for (int colCtr = 0; colCtr < NUM_COLS; ++colCtr)
        {
            keyPressed[rowCtr][colCtr] = false;
            keyTime[rowCtr][colCtr] = 0;
            keyToMidiMap[rowCtr][colCtr] = note;
            note++;
        }
    }
}

void initPins()
{
    // Turn off board LED and midi activity LED.
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);

    // Declare octave transpose buttons.
    pinMode(MINUS_OCTAVE, INPUT);
    pinMode(PLUS_OCTAVE, INPUT);

    // Declare column pins
    for (byte pin = 0; pin < sizeof(output_pins); pin++)
    {
        pinMode(output_pins[pin], OUTPUT);
    }

    // Declare row pins
    for (byte pin = 0; pin < sizeof(input_pins); pin++)
    {
        pinMode(input_pins[pin], INPUT_PULLUP);
    }
}

void readState(Button *btn)
{
    btn->state = digitalRead(btn->pin);
    if (btn->state != btn->lastState && btn->state == HIGH)
    {
        btn->handlePress();
    }
    btn->lastState = btn->state;
}

void readKeys()
{
    // Loop on columns
    for (int colCtr = 0; colCtr < NUM_COLS; ++colCtr)
    {
        // Set column HIGH
        digitalWrite(output_pins[colCtr], HIGH);

        // Read all the rows for that column
        for (int row = 0; row < NUM_ROWS; row++)
        {
            rowValue[row] = digitalRead(input_pins[row]);
        }

        // Set column LOW
        digitalWrite(output_pins[colCtr], LOW);

        // process keys pressed
        // Start by odd rows cause they're hit first.
        for (int rowCtr = 1; rowCtr < NUM_ROWS; rowCtr += 2)
        {
            // If we detect a hit we record the time
            if (rowValue[rowCtr] != 0 && !keyPressed[rowCtr][colCtr])
            {
                keyPressed[rowCtr][colCtr] = true;
                keyTime[rowCtr][colCtr] = micros();
            }

            // Then read the even rows and measure the time difference with the odd row.
            if (rowValue[rowCtr - 1] != 0 && keyTime[rowCtr][colCtr] > 0)
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
            if (rowValue[rowCtr] == 0 && keyPressed[rowCtr][colCtr])
            {
                keyPressed[rowCtr][colCtr] = false;
                noteOff(rowCtr, colCtr);
            }
        }
    }
}

void lowerOctave()
{
    if (octave_offset > -3)
    {
        octave_offset--;
    }
}

void upperOctave()
{
    if (octave_offset < 3)
    {
        octave_offset ++;
    }
}

void noteOn(int row, int col)
{
    // digitalWrite(31, HIGH);
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
    Serial1.write(NOTE_ON_CMD);
    Serial1.write(keyToMidiMap[row][col] + (octave_offset * 12));
    Serial1.write(vel);
}

void noteOff(int row, int col)
{
    // digitalWrite(31, LOW);
    Serial1.write(NOTE_OFF_CMD);
    Serial1.write(keyToMidiMap[row][col] + (octave_offset * 12));
    Serial1.write(0);
}
