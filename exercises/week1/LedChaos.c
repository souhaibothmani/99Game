#include <util/delay.h>
#include <avr/io.h>
#include <led.h>
#include <stdlib.h>


#define NUMBER_OF_LEDS 4 //Define is a "preprocessor directive". It ensures that every NUMBER_OF_LEDS will be replaced by 4 in the following code


int main() {
  srand(time(NULL));
  int ledNumber = rand() % 4;
  enableOneLed(ledNumber);
  lightToggleOneLed(ledNumber);
}