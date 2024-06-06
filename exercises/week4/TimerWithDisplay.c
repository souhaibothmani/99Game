#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "display.h"
#include "usart.h"
#include <button.h>


// Global variables
// Enter here your global variables
int minutes=0;
int seconds=0;
int count =0;
#define MULTIPLE 250


void initTimer() {    

    OCR2A = 249;

    TCCR2A |=  _BV( WGM01 ); //enable CTC mode

    TIMSK2 |= _BV(OCIE2A); //enable OCRA(interrupt)


}

void startTimer() {    

    TCCR2B |= _BV(CS22) | _BV(CS21); //set factor as 256  
    
}

void stopTimer() {    

    TCCR2B &= ~(_BV(CS22) | _BV(CS21)); //set factor as 0 so that it doens't count anymore

}

void tick() { 
    seconds++;
    if((seconds%60)==0){
        minutes++;
        seconds=0;
    }
}

void writeTimeAndWait(uint8_t minuten, uint8_t seconden, int delay) {    
    writeNumberToSegment(0,(minutes / 10) % 10);  // Display tens place of minutes
    writeNumberToSegment(1,minutes % 10);         // Display ones place of minutes
    writeNumberToSegment(2,(seconds / 10) % 10);  // Display tens place of seconds
    writeNumberToSegment(3,seconds % 10);         // Display ones place of seconds
    blankSegment(3); // Otherwise the last segment is much brighter than the rest
  
    
}

// This ISR runs every 4 ms
ISR(TIMER2_COMPA_vect) {// Increment the counter with 1

    count++;
    if(((count+1)%MULTIPLE)==0){// If the counter + 1 is divisible by MULTIPLE, then count 1 sec
        tick();
    } 
    
}

int main() {    
    initUSART();    
    initDisplay();
    initTimer();
    enableAllButtons();
    
    sei();
    printf("Start the stopwatch by pressing button S1, stop by pressing button S2, and reset with S3\n");    
    while (1) {
        if(buttonPushed(1)){
            printf("start Timer\n");
            _delay_ms(1000);
            startTimer();
        }
        if(buttonPushed(2)){
            _delay_ms(1000);
            stopTimer();
        }
        if(buttonPushed(3)){
            _delay_ms(1000);
            seconds=0;
            minutes=0;
        }

        writeTimeAndWait(minutes, seconds, 1000); 

        
        // Don't forget to initialize and start your timer
        // Also, don't forget sei()    
    }
    return 0;
}
