#include <util/delay.h>
#include <avr/io.h>
 
#define NB_BUTTONS 3
#define DEBOUNCING 5000
 
void enableButton (int buttonNumber){
    if (buttonNumber<1||buttonNumber>NB_BUTTONS) return;
    DDRC &= ~(1 << (PC1 + buttonNumber-1));//enable
    PORTC |= (1 << (PC1 + buttonNumber-1));//pull up on
}

void enableAllButtons(){
    for (int i = 1; i <= NB_BUTTONS; ++i) {
        enableButton(i);
    }
}
 
int buttonPushed(int buttonNumber){
    if (buttonNumber<1||buttonNumber>NB_BUTTONS) return 0;  //check if the buttonNumber is correct
    if ((PINC & (1 << (PC1 + buttonNumber-1))) == 0){       //hear the button(check if it's pressed)
       _delay_us(DEBOUNCING);                               //wait to avoid bouncing
       if ((PINC & (1 << (PC1 + buttonNumber-1))) == 0){    //if the button was pressed(even after 1 second==>so still pressed)
           return 1;                                        //return 1
       } else {
         return 0;                                          //else return 0
       }
    }
    return 0;
}
 
int buttonReleased(int buttonNumber) {  
    return 1 - buttonPushed(buttonNumber);    //reverse of buttonPushed
}

void enableBTNinterrupt()
{
    PCICR |= _BV(PCIE1);
    PCMSK1 |= _BV(PC1) | _BV(PC2) | _BV(PC3);
    
}