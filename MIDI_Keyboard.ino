#define NUM_ROWS 8
#define NUM_COLS 8

#define NOTE_ON_CMD 0x90
#define NOTE_OFF_CMD 0x80
#define MAX_VELOCITY 127

#define MIN_TIME 2048
#define MAX_TIME 131072
#define DIFF_TIME (MAX_TIME - MIN_TIME)

#define MINUS_OCTAVE 30
#define PLUS_OCTAVE 32

//MIDI baud rate
#define SERIAL_RATE 31250

typedef struct {
  int pin;
  int state;
  int lastState;
  void (*handlePress)();
} Button;

// Row input pins
byte input_pins[] = {
  2, 3, 4, 5, 6, 7, 8, 9
};

// Column output pins
byte output_pins[] = {
  22, 23, 24, 25, 26, 27, 28, 29
};

Button minus_octave;

int octave_offset = 0;

// Key states and time between 2 signals
bool keyPressed[NUM_ROWS][NUM_COLS];
unsigned long keyTime[NUM_ROWS][NUM_COLS];
uint8_t keyToMidiMap[NUM_ROWS][NUM_COLS];
void readState(Button *btn);
void lowerOctave();

int rowValue[NUM_ROWS];

void setup()
{
  // Turn off board LED and midi activity LED.
  pinMode(13, OUTPUT);
  pinMode(31, OUTPUT);
  digitalWrite(13, LOW);
  digitalWrite(31, LOW);
  // Declare octave transpose buttons.
  pinMode(MINUS_OCTAVE, INPUT);
  pinMode(PLUS_OCTAVE, INPUT);

  minus_octave.pin = MINUS_OCTAVE;
  minus_octave.state = LOW;
  minus_octave.lastState = LOW;
  minus_octave.handlePress = lowerOctave;

  // Init key map.
  int note = 48;
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

  Serial1.begin(SERIAL_RATE);
}

void loop()
{
  readState(&minus_octave);
  
  // Loop on columns
  for (int colCtr = 0; colCtr < NUM_COLS; ++colCtr)
  {
    // Set column HIGH
    digitalWrite(output_pins[colCtr], HIGH);

    // Read all the rows for that column
    for (int row = 0; row < NUM_ROWS; row ++) {
      rowValue[row] = digitalRead(input_pins[row]);
    }

    // Set column LOW
    digitalWrite(output_pins[colCtr], LOW);
    
    // process keys pressed
    // Start by odd rows cause they're hit first.
    for (int rowCtr = 1; rowCtr < NUM_ROWS; rowCtr += 2)
    {
      // If we detect a hit we record the time 
      if (rowValue[rowCtr] != 0 && !keyPressed[rowCtr][colCtr]) {
        keyPressed[rowCtr][colCtr] = true;
        keyTime[rowCtr][colCtr] = micros();
      }

      // Then read the even rows and measure the time difference with the odd row.
      if (rowValue[rowCtr - 1] != 0 && keyTime[rowCtr][colCtr] > 0) {
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

void readState(Button *btn)
{
  btn->state = digitalRead(btn->pin);
  digitalWrite(31, btn->state);
  if (btn->state != btn->lastState && btn->state == HIGH)
  {
    btn->handlePress();
  }
  btn->lastState = btn->state;
}

void lowerOctave()
{
  if (octave_offset > -3)
  {
    octave_offset --;  
  }
}

void noteOn(int row, int col)
{
  // digitalWrite(31, HIGH);
  unsigned long t = keyTime[row][col];
  if (t > MAX_TIME) t = MAX_TIME;
  if (t < MIN_TIME) t = MIN_TIME;

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
