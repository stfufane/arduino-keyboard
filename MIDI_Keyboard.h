#pragma once

#define NUM_ROWS 8
#define NUM_COLS 8

#define NOTE_ON_CMD 0x90
#define NOTE_OFF_CMD 0x80
#define MAX_VELOCITY 127

#define MIN_TIME 2048
#define MAX_TIME 131072
#define DIFF_TIME (MAX_TIME - MIN_TIME)

// Input buttons to switch octaves
#define MINUS_OCTAVE 30
#define PLUS_OCTAVE 32

typedef struct Button
{
    int pin;
    int state;
    int lastState;
    void (*handlePress)();
} Button;

// Key states and time between 2 signals
bool keyPressed[NUM_ROWS][NUM_COLS];
unsigned long keyTime[NUM_ROWS][NUM_COLS];
uint8_t keyToMidiMap[NUM_ROWS][NUM_COLS];

// Row input pins
byte input_pins[] = {
    2, 3, 4, 5, 6, 7, 8, 9};

// Column output pins
byte output_pins[] = {
    22, 23, 24, 25, 26, 27, 28, 29};

Button minus_octave;
Button plus_octave;

int octave_offset = 0;

int rowValue[NUM_ROWS];

void initKeys();
void initPins();
void readState(Button *btn);
void readKeys();
void lowerOctave();
void upperOctave();
void noteOn(int row, int col);
void noteOff(int row, int col);
