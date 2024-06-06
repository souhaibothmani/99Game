#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>
#include <time.h>
#include <led.h>
#include <button.h>
#include <usart.h>

#define LED_COUNT 4
#define NB_BUTTONS 3

void countdown();
void showMorse(char letter);
void displayChoices(char choices[]);
char getAnswer(char choices[]);
void ledDance();

int main() {
    
    initUSART(); //intialise the USART
    srand(time(NULL)); //initialise the random with time=0;

    // enable all LEDs and buttons
    enableAllLeds();
    enableAllButtons();
    //delay of 500ms to keep the leds on
    _delay_ms(500);
    lightDownAllLeds();

    // main loop

    int score = 0;//set the score as 0

    for (int i = 0; i < 5; ++i) { //loop for 5 rounds

        countdown();//countdown before showing the letter to guess

        char letter = 'A' + (rand() % 26);//pick a random letter

        showMorse(letter);//show the letter using the leds(see function)
    
        char choices[3];//array of choices
        choices[0]=letter;
        displayChoices(choices);//display possible choices

        char answer = getAnswer(choices);//retrieve the answer from the player

        // Check if answer is correct
        if (answer == letter) {
            printf("CORRECT!\n");
            score++;

        }else{
            printf("WRONG!\n");
        }

        printf("Correct answer: %c, Your answer: %c\n", letter, answer);
        printf("Score: %d\n",score);
        
        _delay_ms(2000);//give time to the player to read the serial monitor
    
    }

    // Show final score and LED dance
    printf("Final Score: %d\n",score);
    ledDance();

    return 0;
}

void countdown() {//countdown leds
    for (int i = 4; i >= 0; --i) {
        lightUpOneLed(i);
        _delay_ms(500);
        lightDownOneLed(i);
        _delay_ms(500);
    }
}

void showMorse(char letter) {//we pass the letter
    
    const char *morseCode;//we create a pointer morsecode
    switch (letter) {
        case 'A': morseCode = ".-"; break;//we put that morse code as a string inside morseCode following the letter
        case 'B': morseCode = "-..."; break;//acts like an array
        case 'C': morseCode = "-.-."; break;
        case 'D': morseCode = "-.."; break;
        case 'E': morseCode = "."; break;
        case 'F': morseCode = "..-."; break;
        case 'G': morseCode = "--."; break;
        case 'H': morseCode = "...."; break;
        case 'I': morseCode = ".."; break;
        case 'J': morseCode = ".---"; break;
        case 'K': morseCode = "-.-"; break;
        case 'L': morseCode = ".-.."; break;
        case 'M': morseCode = "--"; break;
        case 'N': morseCode = "-."; break;
        case 'O': morseCode = "---"; break;
        case 'P': morseCode = ".--."; break;
        case 'Q': morseCode = "--.-"; break;
        case 'R': morseCode = ".-."; break;
        case 'S': morseCode = "..."; break;
        case 'T': morseCode = "-"; break;
        case 'U': morseCode = "..-"; break;
        case 'V': morseCode = "...-"; break;
        case 'W': morseCode = ".--"; break;
        case 'X': morseCode = "-..-"; break;
        case 'Y': morseCode = "-.--"; break;
        case 'Z': morseCode = "--.."; break;
        default: morseCode = ""; break;
    }

    while (*morseCode) {//as long as this is not null
        
        //char at the value of that address

        if (*morseCode == '.') {//if it's a '.' light up for 200ms
            lightUpAllLeds();
            _delay_ms(200);    
            lightDownAllLeds();
            _delay_ms(200);
        } 
        else if (*morseCode == '-') {//if it's a '-' light up for 600
            lightUpAllLeds();
            _delay_ms(600);
            lightDownAllLeds();
            _delay_ms(200);
        }

        morseCode++;//go to next char
    }
}

void displayChoices(char choices[]) {
    
    do {
        choices[1] = 'A' + (rand() % 26);//retrieve a random letter as long as it's not the same as the correct answer
    } while (choices[1] == choices[0]);
    do {
        choices[2] = 'A' + (rand() % 26);//retreive a random letter as long as it's not the same as the correct answer and the previous random letter
    } while (choices[2] == choices[0] || choices[2] == choices[1]);

   
    for (int i = 0; i < 3; ++i) { // shuffle the choices
        int j = rand() % 3;
        char temp = choices[i];
        choices[i] = choices[j];
        choices[j] = temp;
    }

    printf("Choose the correct letter:\n");//ask to enter a letter
    printf("A: %c\n", choices[0]);
    printf("B: %c\n", choices[1]);
    printf("C: %c\n", choices[2]);
}

char getAnswer(char choices[]) {
    while (1) {//wait for the player to make a choice
        if (buttonPushed(1)) {
            return choices[0];
        } else if (buttonPushed(2)) {
            return choices[1];
        } else if (buttonPushed(3)) {
            return choices[2];
        }
    }
}


void ledDance() {
    for (int i = 4; i >= 0; --i) {
        lightUpOneLed(i);
        _delay_ms(100);
        lightDownOneLed(i);
        _delay_ms(100);
    }
    for (int i = 4; i >= 0; --i) {
        lightUpOneLed(i);
        _delay_ms(100);
        lightDownOneLed(i);
        _delay_ms(100);
    }
    
    for (int i = 0; i < 10; ++i) {
        lightUpAllLeds();
        _delay_ms(200);
        lightDownAllLeds();
        _delay_ms(200);
    }
    for (int i = 4; i >= 0; --i) {
        lightUpOneLed(i);
        _delay_ms(100);
        lightDownOneLed(i);
        _delay_ms(100);
    }
    for (int i = 4; i >= 0; --i) {
        lightUpOneLed(i);
        _delay_ms(100);
        lightDownOneLed(i);
        _delay_ms(100);
    }
}
