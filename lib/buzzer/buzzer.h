#include <avr/io.h>
#include <util/delay.h>

// Function to enable the buzzer
void enableBuzzer();

// Function to play a tone with a given frequency and duration
void playTone(float frequency, uint32_t duration);

// Function to disable the buzzer
void disableBuzzer();

