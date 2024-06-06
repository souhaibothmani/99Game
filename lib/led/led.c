// to allow variables as parameter for the _delay-functions (must be placed before the include of delay.h)
#define __DELAY_BACKWARD_COMPATIBLE__  
#include <util/delay.h> 

#include <avr/io.h>
#include <stdint.h>

 
#define LED_COUNT 4

//---------------------------------------------------**ENABLE**-----------------------------------------------

// Function to enable a single LED
void enableOneLed(int led) {
    DDRB |= (1 << (PB2 + led));
}

// Function to enable multiple LEDs based on an 8-bit value
void enableMultipleLeds(uint8_t leds) {
    for (int i = 0; i < LED_COUNT; ++i) {
        if (leds & (1 << i)) {
            enableOneLed(i);
        }
    }
}

// Function to enable all LEDs
void enableAllLeds() {
    for (int i = 0; i < LED_COUNT; ++i) {
        enableOneLed(i);
    }
}

//---------------------------------------------------**LIGHT UP**-----------------------------------------------

void lightUpOneLed(int led){
    PORTB &= ~( 1 << ( PB2 + led ));//light up a led
}

void lightUpMultipleLeds(uint8_t leds){
        for(int i = 0; i < LED_COUNT; ++i) {
        if (leds & (1 << i)) {
            lightUpOneLed(i);
        }
    }
}
void lightUpAllLeds(){
        for (int i = 0; i < LED_COUNT; ++i) {
        lightUpOneLed(i);
    }
}

//---------------------------------------------------**LIGHT DOWN**-----------------------------------------------

void lightDownOneLed(int led){
     PORTB |= ( 1 << ( PB2 + led )); //light down a led
}
void lightDownMultipleLeds(uint8_t leds){
 for (int i = 0; i < LED_COUNT; ++i) {
        if (leds & (1 << i)) {
            lightDownOneLed(i);
        }
    }
}
void lightDownAllLeds(){
        for (int i = 0; i < LED_COUNT; ++i) {
        lightDownOneLed(i);
    }
}

//---------------------------------------------------**OTHER FUNCTIONS**-----------------------------------------------

void lightToggleOneLed(int led){//toggle a led 
      while (1)
    {
        lightUpOneLed(led);
        _delay_ms(1000); //1 sec
        lightDownOneLed(led); 
        _delay_ms(1000);
    }
    return 0;
}

void dimLed(int lednumber, int percentage, int total_duration) {
  // Error handling: Check for valid percentage (0-100)
  if (percentage < 0 || percentage > 100) {
    return printf("wrong percentage"); 
  }

  // Calculate on and off times based on the percentage
  int cycle_duration = 10; // Define a short cycle duration for smooth dimming (in ms)
  int on_time = (cycle_duration * percentage) / 100;
  int off_time = cycle_duration - on_time;

  // Calculate the number of cycles to cover the total duration
  int cycles = total_duration / cycle_duration;

  for (int i = 0; i < cycles; i++) {
    lightUpOneLed(lednumber);
    _delay_ms(on_time);
    lightDownOneLed(lednumber);
    _delay_ms(off_time);
  }
}

void fadeInLed(int lednumber, int duration) {
  int steps = 50; // Number of steps for fading in
  int step_delay = duration / steps; // Duration for each step

  for (int brightness = 0; brightness <= 100; brightness += 2) {//each step add the brightness by 1
    dimLed(lednumber, brightness, step_delay);
  }
}

void fadeOutLed(int lednumber, int duration){
    int steps = 50;
    int step_delay = duration / steps;
      for (int brightness = 100; brightness >= 0; brightness -= 2) {
    dimLed(lednumber, brightness, step_delay); // Adjust duration for each step
  }

}

