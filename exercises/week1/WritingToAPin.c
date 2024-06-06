#include <avr/io.h> //this library includes the definitions of the registers (DDRB, PORTB, ...)

int main(){ 
   DDRB = 0b00001000;//enable the pin 3 of port B for output (count from right to left, start from 0)
   PORTB = 0b00001000;//we apply voltage to the 3rd pin of port B - this turns a led OFF(!)
   return 0;
}