#include "MIDI_Keyboard.h"

// MIDI baud rate
#define SERIAL_RATE 31250

void setup()
{
    initKeys();
    initPins();

    Serial1.begin(SERIAL_RATE);
}

void loop()
{
    readState(&minus_octave);
    readState(&plus_octave);

    readKeys();
}
