#include <MIDI.h>
#include "Keyboard.h"

// This inits the midi interface and creates an instance.
MIDI_CREATE_DEFAULT_INSTANCE();

// Keyboard class declares all the inputs and reads them.
Keyboard keyboard(MIDI);

void setup()
{
    // Start MIDI on serial port.
    MIDI.begin();

    // Add a short delay before reading the first values.
    delay(200);
    keyboard.setup();
}

void loop()
{
    keyboard.checkValues();
}
