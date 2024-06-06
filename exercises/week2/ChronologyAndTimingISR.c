#include <util/delay.h>
#include <avr/io.h>
#include <usart.h>
#include <led.h>
#include <button.h>
#include <avr/interrupt.h>

ISR( PCINT1_vect )
{
    if ( bit_is_clear( PINC, PC1 ))//check if s1 is pressed
    {
    
        _delay_us( 1000 );//debounce

        if ( bit_is_clear( PINC, PC1 ))//check again
        {   
            enableOneLed(3);
        
            lightUpOneLed(2);
            lightUpOneLed(3);
        
            _delay_ms(5000);
    
            lightDownOneLed(2);
            lightDownOneLed(3);

            _delay_ms(5000);
        
        }
    }
}

int main(){
    enableOneLed(1);
    enableOneLed(2);
    

    enableButton(1);
    

    PCICR |= _BV(PCIE1);
    PCMSK1 |= _BV(PC1);

    sei();

    while (1)
    {
        lightUpOneLed(1);
        lightUpOneLed(2);
        
        _delay_ms(2000);
    
        lightDownOneLed(1);
        lightDownOneLed(2);

        _delay_ms(2000);
        
    }


    
}