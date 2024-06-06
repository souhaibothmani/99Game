#include <util/delay.h>
#include <avr/io.h>
#include <led.h>

#define NUMBER_OF_LEDS 4 //Define is a "preprocessor directive". It ensures that every NUMBER_OF_LEDS will be replaced by 4 in the following code


int main() {
  enableOneLed(1);

  while (1)
  {
    fadeInLed(1,1000);
  }
 
}