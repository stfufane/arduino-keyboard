#include <MIDI.h>
#include "Debug.h"
#include "Keyboard.h"

// This inits the midi interface on serial1 and creates an instance.
MIDI_CREATE_DEFAULT_INSTANCE();

// Keyboard class declares all the inputs and reads them.
Keyboard keyboard(MIDI);

void setup()
{
    // Start MIDI interface.
    MIDI.begin();

    // Init serial port for debugging
    Serial.begin(9600);
    DBG("Setup in progress");

    // Add a short delay before reading the first values.
    delay(200);
    keyboard.setup();

    DBG("Setup done!");
}

void loop()
{
    keyboard.checkValues();
}
