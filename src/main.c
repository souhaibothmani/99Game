#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>
#include <usart.h>
#include <display.h>
#include <stdlib.h> // For rand(), malloc, realloc, free
#include <button.h>
#include <led.h>
#include <potentiometer.h>
#include <buzzer.h>
#include <timer.h>

//******************************************************************************************************************************
//                                                     DECLARATIONS
//******************************************************************************************************************************


#define WIN_SCORE 99 //used to check if the player/computer won
#define INITIAL_TURNS_CAPACITY 10 //used for array capacity 
#define MULTIPLE 250//used to keep track of seconds

typedef struct {//struc to keep track of each turn
    char player; // 'P' or 'C'
    int rolls[10]; // Store up to 10 rolls per turn
    int numRolls;
    int turnScore;
    int playerScore;
    int computerScore;
    char decision; // 'A' for add, 'S' for subtract
} GameTurn;

GameTurn* gameTurns = NULL; //array of turns
int currentTurnIndex = 0; 
int turnsCapacity = 0;

int playerScore = 0; // player score
int computerScore = 0; // computer score
int currentTurnScore = 0; // current turn score
char currentPlayer = 'P'; // 'P' for Player, 'C' for Computer
int roll = 0; //current roll value
int count = 0; 
int seconds = 0;
int buttonIsPressed = 0;

//******************************************************************************************************************************
//|                                                  METHODS                                                             
//******************************************************************************************************************************


//******************************************************************************************************************************
//                                                DISPLAY METHODS

void displayGameStatus() {
    for (int i = 0; i < 10; i++) {
        if (currentPlayer == 'P') { // display player's total score
            writeCharToSegment(1, 'P');
            writeNumberToSegment(2, playerScore / 10);
            writeNumberToSegment(3, playerScore % 10);
        } else { // display computer's total score
            writeCharToSegment(1, 'C');
            writeNumberToSegment(2, computerScore / 10);
            writeNumberToSegment(3, computerScore % 10);
        }
    }
}

void displayTurnStatus() {
    for (int i = 0; i < 10; i++) {
        if (currentPlayer == 'P') { // display player's turn score
            writeCharToSegment(1, 'P');
            writeNumberToSegment(2, currentTurnScore / 10);
            writeNumberToSegment(3, currentTurnScore % 10);
        } else { // display computer's turn score
            writeCharToSegment(1, 'C');
            writeNumberToSegment(2, currentTurnScore / 10);
            writeNumberToSegment(3, currentTurnScore % 10);
        }
    }
}

void displayRoll() {
    if (currentPlayer == 'P') { // display player's roll result
        for (int i = 0; i < 500; i++) {
            writeNumberToSegment(0, roll);
            writeCharToSegment(1, 'P');
            writeNumberToSegment(2, currentTurnScore / 10);
            writeNumberToSegment(3, currentTurnScore % 10);
        }
    } else { // display computer's roll result
        for (int i = 0; i < 500; i++) {
            writeNumberToSegment(0, roll);
            writeCharToSegment(1, 'C');
            writeNumberToSegment(2, currentTurnScore / 10);
            writeNumberToSegment(3, currentTurnScore % 10);
        }
    }
}

void displayGameLog() {
    for (int i = 0; i < currentTurnIndex; i++) {
        printf("Turn %d: Player: %c, Rolls: ", i + 1, gameTurns[i].player);
        for (int j = 0; j < gameTurns[i].numRolls; j++) {
            printf("%d ", gameTurns[i].rolls[j]);
        }
        printf(", Turn Score: %d, Player Score: %d, Computer Score: %d, Decision: %c\n", 
            gameTurns[i].turnScore, gameTurns[i].playerScore, gameTurns[i].computerScore, gameTurns[i].decision);
    }
}

//******************************************************************************************************************************





//******************************************************************************************************************************
//                                              GAME LOOP METHODS
void handlePlayerTurn() { //handle the player turn
    while (currentPlayer == 'P') { //as long as the player playing is P(so us)

        //we start by displaying the turn status
        displayTurnStatus(); 

        if (buttonPushed(1)) { // if player presses button 1 
            roll = (rand() % 6) + 1; // we roll by using a random number
            for (int i = 0; i < 10; i++) {
                displayRoll(); // and then we display the roll result
            }

            if (roll == 1) { // if roll is 1, reset turn score and switch turn
                currentTurnScore = 0;
                switchTurnAfter1();
            } else { 
                currentTurnScore += roll; // we add the roll value to the turn score
            }
        }

        if (buttonPushed(2)) { // if player pressed button 2
            _delay_ms(100); //debounce
            while (1) { //as long as the player didn't press any other button

                displayTurnStatus(); //we display the turn value

                if (buttonPushed(2)) { //if button 2 is pressed
                    playerScore += currentTurnScore; // add turn score to total score
                    logGameTurn('A'); //save the turn as A(add)

                    if (playerScore >= WIN_SCORE) { //if player score is higher or equal to the turn score

                        writeStringAndWait("PWIN", 1000); // Display "PWIN" for 1 second
                        displayGameLog(); //display all the turns on the serial monitor
                        enableBuzzer(); //enable the buzzer 
                        playTone(1046.502, 1); // , play sound for winning for 1 second
                        free(gameTurns); // and free the allocated memory

                        while (1); // stop the game
                    }
                    switchTurn();//switch the Turn
                    break;
                }
                if (buttonPushed(3)) { //if player pressed button 3
                    decrementOpponentScore(&computerScore);// subtract from opponent's score by passing the address of the variable
                    logGameTurn('S'); //save turn as S(substract)
                    switchTurn(); //switch turn
                    break;
                }
            }
        }
    }
}

void handleComputerTurn() { //handle computer turn
    while (currentPlayer == 'C') { //as long as it's the computer turn

        for (int i = 0; i < 1000; i++) {
            displayTurnStatus(); // display current turn status
        }

        roll = (rand() % 6) + 1; // generate random number as roll
        for (int i = 0; i < 10; i++) {
            displayRoll(); // display roll result
        }

        if (roll == 1) {
            currentTurnScore = 0;
            switchTurnAfter1();
        } else {
            currentTurnScore += roll;
        }

        // intelligent decision-making
        if (currentTurnScore < 10) {
            // roll again if turn score is less than 10
            continue;
        } else if (currentTurnScore > 15) {
            // end turn if turn score exceeds 15
            break;
        } else if (currentTurnScore >= 10 && currentTurnScore <= 15) {
            // 50% chance to roll again if turn score is between 10 and 15
            if ((rand() % 100) < 50) {
                continue;
            } else {
                break;
            }
        }
    }

    if (currentPlayer == 'C') {
        computerScore += currentTurnScore;
        if (playerScore > 80 && (rand() % 100) < 80) { // 80% chance to subtract from player's score
            decrementOpponentScore(&playerScore);//decrement opponent's score
        }
        logGameTurn('A');
        if (computerScore >= WIN_SCORE) {
            writeStringAndWait("CWIN", 1000); // Display "CWIN" for 1 second
            displayGameLog(); 
            enableBuzzer();
            playTone(1046.502, 1); // Play sound for winning
            free(gameTurns); // Free the allocated memory
            while (1); // Stop the game
        }
        switchTurn();
    }
}

//******************************************************************************************************************************




//******************************************************************************************************************************
//                                              SWITCH TURN METHODS
void switchTurn() {
    currentTurnScore = 0; //set the current turnscore to 0
    roll = 0; //set the roll to 0
    for (int i = 0; i < 500; i++) {
        displayGameStatus(); // display updated game status
    }
    currentPlayer = (currentPlayer == 'P') ? 'C' : 'P'; //change the currentPlayer
    for (int i = 0; i < 500; i++) {
        displayGameStatus(); // display updated game status of the other player before starting the turn
    }
}

void switchTurnAfter1() { //same as switchTurn but with buzzer
    currentTurnScore = 0;
    roll = 0;
    for (int i = 0; i < 500; i++) {
        enableBuzzer();
        playTone(783.990, 1);
        displayGameStatus(); // display updated game status
    }
    currentPlayer = (currentPlayer == 'P') ? 'C' : 'P';
    for (int i = 0; i < 500; i++) {
        displayGameStatus(); // display updated game status
    }
}

void flashLEDs(int times, int delay) {
    for (int i = 0; i < times; i++) {
        enableAllLeds();
        lightUpAllLeds();
        _delay_ms(delay);
        lightDownAllLeds();
        _delay_ms(delay);
    }
}

void decrementOpponentScore(int* score) {//declare a pointer
    // initial display update
    for (int j = 0; j < 5000; j++) {
        if (currentPlayer == 'P') {
            writeNumberToSegment(0, currentTurnScore);
            writeCharToSegment(1, 'C');
            writeNumberToSegment(2, computerScore / 10);
            writeNumberToSegment(3, computerScore % 10);
        } else {
            writeNumberToSegment(0, currentTurnScore);
            writeCharToSegment(1, 'P');
            writeNumberToSegment(2, playerScore / 10);
            writeNumberToSegment(3, playerScore % 10);
        }
    }
    // decrement loop
    for (int i = 1; i < currentTurnScore + 1; i++) {
        if (*score < 0) *score = 0; //if score is less than 0 then score =0;
        (*score)--;//we gonna substract the value inside the box that the pointer points at

        // display update after decrement
        for (int j = 0; j < 5000; j++) {
            if (currentPlayer == 'P') {
                writeNumberToSegment(0, currentTurnScore - i);
                writeCharToSegment(1, 'C');
                writeNumberToSegment(2, computerScore / 10);
                writeNumberToSegment(3, computerScore % 10);
            } else {
                writeNumberToSegment(0, currentTurnScore - i);
                writeCharToSegment(1, 'P');
                writeNumberToSegment(2, playerScore / 10);
                writeNumberToSegment(3, playerScore % 10);
            }
        }
        // flash LEDs on
        flashLEDs(1, 187.5);
    }
}

void logGameTurn(char decision) {
    if (currentTurnIndex >= turnsCapacity) {
        // resize the gameTurns array if needed
        turnsCapacity = (turnsCapacity == 0) ? INITIAL_TURNS_CAPACITY : turnsCapacity * 2;
        gameTurns = realloc(gameTurns, turnsCapacity * sizeof(GameTurn));//==> turnsCapacity times the GameTurn(so each turn)==>
                                                                        //so for example at the beginning we have 10(turnsCapacity)* the sizeof a gameTurn(so in total we will have the size of 10 GameTurns)
        if (!gameTurns) {
            // handle allocation failure
            writeStringAndWait("MEMERR", 1000);
            while (1); // stop the game
        }
    }
    gameTurns[currentTurnIndex].player = currentPlayer;
    gameTurns[currentTurnIndex].turnScore = currentTurnScore;
    gameTurns[currentTurnIndex].playerScore = playerScore;
    gameTurns[currentTurnIndex].computerScore = computerScore;
    gameTurns[currentTurnIndex].decision = decision;
    currentTurnIndex++;
}
//******************************************************************************************************************************



//******************************************************************************************************************************
//                                              INITIALIZATION METHODS

void PotentioValue(){ //read the value from the potentiometer
    OCR2A = 249;//prescaler is set to 256==> counts every 16micro==> after how many counts do we have 4 mili seconds? 4ms/16us==>250 but
                //because timer counts from 0 we have to set it as 249
    PCICR |= _BV(PCIE1);  /* in Pin Change Interrupt Control Register: indicate
                             * which interrupt(s) you want to activate (PCIE0: port B,
                             * PCIE1: port C, PCIE2: port D) */

    PCMSK1 |= _BV(PC1);   /* In the corresponding Pin Change Mask Register: indicate
                             * which pin(s) of that port activate the ISR. */

    startTimer(); //start the timer 

    while (seconds < 30) { // Wait for 30 seconds or button press to exit the loop
        uint16_t potentio = readPotentio();//read value from potentio
        writeNumber(potentio); //write the value of the potentio in the display

        if (buttonIsPressed) {//check if button is pressed
            break;
        }
    }
    stopTimer();//we stop the timer
}

ISR(TIMER2_COMPA_vect) {//interrupt for every time we reach the timer reaches the value of OCR==249(and not overflow)
    count++;//count everytime this interrupt
    if(((count+1)%MULTIPLE)==0){// if the counter + 1 is divisible by MULTIPLE, then count 1 sec(250 times 4ms==>1000ms==>1s)
        seconds++;
        printf("seconds left = %d\n", 30-seconds); //printf the seconds left before we exit the loop
    } 
}

ISR(PCINT1_vect) {//interrupt for when button 1 is pressed
    // button 1 is pressed (bit is set to 0)?
    if (bit_is_clear(PINC, PC1)) { //check if button1 is pressed with bit is clear(so equals 0)
        //we wait 250 microseconds and check again (debounce!)
        _delay_us(250);
  
        if (bit_is_clear(PINC, PC1)) {//check again
            buttonIsPressed = 1; //if same value then we set the boolean buttonIsPressed as true
        }
    }
}
//******************************************************************************************************************************




//******************************************************************************************************************************
//                                                    MAIN
//******************************************************************************************************************************

int main() {
    enableAllLeds(); //enable all the leds
    lightDownAllLeds(); //light down all the leds
    initUSART();    //initialize the USART
    initDisplay();  //initialize the display
    enableAllButtons(); //enable all the buttons
    enablePotentio(); //enable potentiometer
    initTimer();    //initialize the timer

    PotentioValue();// read the value from the potentiometer for 30 seconds and display it. if button 2 was pressed ==> exit the loop nad retrieve the value

    srand(readPotentio()); // seed the random number generator with the value of the potentiometer

    currentPlayer = (rand() % 2 == 0) ? 'P' : 'C'; // decide who starts randomly

    for (int i = 0; i < 500; i++) {//display the game status at the beginning of the game
        displayGameStatus();
    }
    while (1) { // main loop
        if (currentPlayer == 'P') { //if player P then call
            handlePlayerTurn();  //this
        } else {                   //if player C then call
            handleComputerTurn(); //this
        }
    }

    return 0;
}
