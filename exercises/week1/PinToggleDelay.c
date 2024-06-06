#include <avr/io.h>   //Library containing the arduino register definitions
#include <util/delay.h> //Library containing the delay (~sleep) function

int main(){ 
 DDRB = 0b00010000;  //The 3rd led is connected to PB4, numbered from right to left (start counting from 0)
 while (1){          //A while 1 loop to blink "forever"  
   PORTB = 0b00000000;//we don't apply voltage to PB4, so there is a voltage difference, by consequence the led lights up
   _delay_ms(1000);
   PORTB = 0b00010000;//we apply voltage to PB4, so the voltage difference becomes 0, by consequence the led goes dark
   _delay_ms(1000);
   }
return 0;
}