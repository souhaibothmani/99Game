#include <stdint.h>

void enableOneLed(int led);
void enableMultipleLeds(uint8_t leds);
void enableAllLeds();

void lightUpOneLed(int led);
void lightUpMultipleLeds(uint8_t leds);
void lightUpAllLeds();

void lightDownOneLed(int led);
void lightDownMultipleLeds(uint8_t leds);
void lightDownAllLeds();

void lightToggleOneLed(int led);
void dimLed (int lednumber, int percentage, int duration);
void fadeInLed (int led, int duration);
void fadeOutLed(int, int);