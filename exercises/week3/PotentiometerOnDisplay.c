#include <util/delay.h>
#include <avr/io.h>
#include <usart.h>
#include <potentiometer.h>
#include <display.h>

int main()
{
    initUSART();
    enablePotentio();
    initDisplay();
    while ( 1 )
    {
        uint16_t potentio = readPotentio();

        printf("%d\n",potentio);
        writeNumberAndWait(potentio,100);
        
    
    }
    return 0;
}