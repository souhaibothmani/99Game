#include <timer.h>

void initTimer()
{
    // enter your code
    // TCCR2A |= _BV(WGM21);| _BV(WGM20);
    TCCR2A |= _BV(WGM22) | _BV(WGM20);
    TIMSK2 |= _BV(OCIE2A);
    sei();
}

void startTimer()
{
    // enter your code
    TCCR2B |= _BV(CS22) | _BV(CS20);
}

void stopTimer()
{
    // enter your code
    TCCR2B &= ~(_BV(CS22) | _BV(CS20));
}

void writeTimeAndWait(uint8_t minuten, uint8_t seconden, int delay)
{
    // enter your code
    for (uint8_t i = 0; i < delay; i++)
    {
        writeNumberToSegment(0, minuten / 10 % 10);
        _delay_ms(5);
        writeNumberToSegment(1, minuten % 10);
        _delay_ms(5);
        writeNumberToSegment(2, seconden / 10 % 10);
        _delay_ms(5);
        writeNumberToSegment(3, seconden % 10);
        _delay_ms(5);
    }
}