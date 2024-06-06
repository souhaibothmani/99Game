#include <usart.h>
#include <button.h>
#include <avr/io.h>
#include <display.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define MULTIPLE 250

void stopTimer();
void initTimer();
void startTimer();
// void tick();?
// void tick(uint8_t *seconds, uint8_t *minutes);
void writeTimeAndWait(uint8_t minuten, uint8_t seconden, int delay);