#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <button.h>
#include <led.h>

#define LED_PORT PORTB
#define BUTTON_PORT PORTC
#define BUTTON_PIN PINC
#define LED_DDR DDRB
#define BUTTON_DDR DDRC
#define BUTTON1 PC1
#define LED1 PB2
#define LED2 PB3

/* First, review the code in the main function!
 * This ISR is called when Pin Change Interrupt 1 is triggered (PCINT1_vect)
 * This is the interrupt for PORTC, where the switches/buttons are connected... */
ISR( PCINT1_vect )
{
    // button 1 is pressed (bit is set to 0)?
    if ( bit_is_clear( BUTTON_PIN, BUTTON1 ))//check using button c is button1 is pressed with bit is clear(so equals 0)
    {
        //We wait 1000 microseconds and check again (debounce!)
        _delay_us( 1000 );
        // button 0 is pressed (bit is set to 0)?
        if ( bit_is_clear( BUTTON_PIN, BUTTON1 ))
        {
            if ( bit_is_set( LED_PORT, LED2 ))  // if led2 is off (bit is set to 1)
            {
                lightUpOneLed(2);       // turn led2 on (set bit to 0)
            }
            else                                // else
            {
                lightDownOneLed(2);     // turn led2 off (set bit to 1)
            } 
        }
    }
}

int main()
{
    enableOneLed(1);   // we'll use led1 and led2
    enableOneLed(2);

    lightDownOneLed(1); //light down both leds
    lightDownOneLed(2);

    enableButton(0);          // we'll use button1
                             // enable pull-up(enableButton does both)

    PCICR |= _BV( PCIE1 );  /* in Pin Change Interrupt Control Register: indicate
                             * which interrupt(s) you want to activate (PCIE0: port B,
                             * PCIE1: port C, PCIE2: port D) */

    PCMSK1 |= _BV( BUTTON1 );   /* In the corresponding Pin Change Mask Register: indicate
                                 * which pin(s) of that port activate the ISR. */

    sei();  // Set Enable Interrupts --> activate the interrupt system globally.

    while ( 1 )
    {
        lightUpOneLed(1);   // turn led1 on
        _delay_ms( 100 );
        lightDownOneLed(1);    // turn led1 off
        _delay_ms( 100 );
    }
    return 0;
}