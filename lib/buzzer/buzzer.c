#include <util/delay.h>
#include <avr/io.h>

// Function to enable the buzzer
void enableBuzzer() {
    DDRD |= (1 << PD3); // Set PD3 as an output pin for the buzzer
}

// Function to play a tone with a given frequency and duration
void playTone(float frequency, uint32_t duration) {
    uint32_t periodInMicro = (uint32_t)(1000000 / frequency); // Calculate the period in microseconds from the frequency
    uint32_t durationInMicro = duration * 1000; // Convert duration from milliseconds to microseconds

    for (uint32_t time = 0; time < durationInMicro; time += periodInMicro) {
        PORTD &= ~(1 << PD3); // Turn the buzzer on
        for (uint32_t i = 0; i < (periodInMicro / 2); i++) {
            _delay_us(1);
        }
        PORTD |= (1 << PD3); // Turn the buzzer off
        for (uint32_t i = 0; i < (periodInMicro / 2); i++) {
            _delay_us(1);
        }
    }
}

// Function to disable the buzzer
void disableBuzzer() {
    DDRD &= ~(1 << PD3); // Set PD3 as an input pin to disable the buzzer
}
