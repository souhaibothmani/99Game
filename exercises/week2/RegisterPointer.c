#include <usart.h>
#include <string.h>
#include <led.h>
#include <avr/io.h>

int main()
{
    int lednumber = 2; 

    uint8_t bitmask = (1 << (PB2 + lednumber));  // creating the bitmask

    writeMask(&DDRB, bitmask);  //pass the address of the register and the bitmask to enable the led

    return 0;
}

void writeMask(volatile uint8_t* reg, uint8_t bitmask)
{
    *reg = bitmask;  // modify the register by setting the bits in bitmask
}
// write value LOW (0) or HIGH (1) to a specific digital pin on a specific register
void writePin( volatile uint8_t* regist, uint8_t pin, uint8_t value ){
    
}
