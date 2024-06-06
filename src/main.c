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

#define WIN_SCORE 99
#define INITIAL_TURNS_CAPACITY 10

typedef struct {
    char player; // 'P' or 'C'
    int rolls[10]; // Store up to 10 rolls per turn
    int numRolls;
    int turnScore;
    int playerScore;
    int computerScore;
    char decision; // 'A' for add, 'S' for subtract
} GameTurn;

GameTurn* gameTurns = NULL;
int currentTurnIndex = 0;
int turnsCapacity = 0;

int playerScore = 0; // player score
int computerScore = 0; // computer score
int currentTurnScore = 0; // current turn score
char currentPlayer = 'P'; // 'P' for Player, 'C' for Computer
int roll = 0;
int count = 0;
int seconds = 0;
int buttonIsPressed = 0;

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

void switchTurn() {
    currentTurnScore = 0;
    roll = 0;
    for (int i = 0; i < 500; i++) {
        displayGameStatus(); // display updated game status
    }
    currentPlayer = (currentPlayer == 'P') ? 'C' : 'P';
    for (int i = 0; i < 500; i++) {
        displayGameStatus(); // display updated game status
    }
}

void switchTurnAfter1() {
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

void decrementOpponentScore(int* score) {
    // Initial display update
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
    // Decrement loop
    for (int i = 1; i < currentTurnScore + 1; i++) {
        if (*score < 0) *score = 0;
        (*score)--;

        // Display update after decrement
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
        // Flash LEDs on
        flashLEDs(1, 187.5);
    }
}

void logGameTurn(char decision) {
    if (currentTurnIndex >= turnsCapacity) {
        // Resize the gameTurns array if needed
        turnsCapacity = (turnsCapacity == 0) ? INITIAL_TURNS_CAPACITY : turnsCapacity * 2;
        gameTurns = realloc(gameTurns, turnsCapacity * sizeof(GameTurn));
        if (!gameTurns) {
            // Handle allocation failure
            writeStringAndWait("MEMERR", 1000);
            while (1); // Stop the game
        }
    }
    gameTurns[currentTurnIndex].player = currentPlayer;
    gameTurns[currentTurnIndex].turnScore = currentTurnScore;
    gameTurns[currentTurnIndex].playerScore = playerScore;
    gameTurns[currentTurnIndex].computerScore = computerScore;
    gameTurns[currentTurnIndex].decision = decision;
    currentTurnIndex++;
}

void handlePlayerTurn() {
    while (currentPlayer == 'P') {
        displayTurnStatus(); // display current turn status

        if (buttonPushed(1)) { // player rolls the dice
            roll = (rand() % 6) + 1; // generate a random number
            for (int i = 0; i < 10; i++) {
                displayRoll(); // display roll result
            }

            if (roll == 1) { // if roll is 1, reset turn score and switch turn
                currentTurnScore = 0;
                switchTurnAfter1();
            } else {
                currentTurnScore += roll; // add roll to turn score
            }
        }

        if (buttonPushed(2)) { // player ends their turn
            _delay_ms(100);
            while (1) {
                displayTurnStatus();

                if (buttonPushed(2)) {
                    playerScore += currentTurnScore; // add turn score to total score
                    logGameTurn('A');
                    if (playerScore >= WIN_SCORE) {
                        writeStringAndWait("PWIN", 1000); // Display "PWIN" for 1 second
                        displayGameLog();
                        enableBuzzer();
                        playTone(1046.502, 1); // Play sound for winning
                        free(gameTurns); // Free the allocated memory
                        while (1); // Stop the game
                    }
                    switchTurn();
                    break;
                }
                if (buttonPushed(3)) { // subtract from opponent's score
                    _delay_ms(250);
                    decrementOpponentScore(&computerScore);
                    logGameTurn('S');
                    switchTurn();
                    break;
                }
            }
        }
    }
}

void handleComputerTurn() {
    while (currentPlayer == 'C') {
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

        // Intelligent decision-making
        if (currentTurnScore < 10) {
            // Roll again if turn score is less than 10
            continue;
        } else if (currentTurnScore > 15) {
            // End turn if turn score exceeds 15
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
            decrementOpponentScore(&playerScore);
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

// TIMER
#define MULTIPLE 250

void initTimer() {
    OCR2A = 249;
    TCCR2A |= _BV(WGM01); //enable CTC mode
    TIMSK2 |= _BV(OCIE2A); //enable OCRA(interrupt)
}

void startTimer() {
    TCCR2B |= _BV(CS22) | _BV(CS21); //set factor as 256  
}

void stopTimer() {
    TCCR2B &= ~(_BV(CS22) | _BV(CS21)); //set factor as 0 so that it doesn't count anymore
}

ISR(TIMER2_COMPA_vect) {
    count++;
    if(((count+1)%MULTIPLE)==0){// If the counter + 1 is divisible by MULTIPLE, then count 1 sec
        seconds++;
        printf("seconds left = %d\n", 30-seconds);
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

ISR(PCINT1_vect) {
    // button 1 is pressed (bit is set to 0)?
    if (bit_is_clear(PINC, PC1)) { //check using button c is button1 is pressed with bit is clear(so equals 0)
        //We wait 250 microseconds and check again (debounce!)
        _delay_us(250);
        // button 0 is pressed (bit is set to 0)?
        if (bit_is_clear(PINC, PC1)) {
            buttonIsPressed = 1;
        }
    }
}

int main() {
    enableAllLeds();
    lightDownAllLeds();
    initUSART();
    initDisplay();
    enableAllButtons();
    enablePotentio();
    initTimer();
    OCR2A = 249;

    PCICR |= _BV(PCIE1);  /* in Pin Change Interrupt Control Register: indicate
                             * which interrupt(s) you want to activate (PCIE0: port B,
                             * PCIE1: port C, PCIE2: port D) */

    PCMSK1 |= _BV(PC1);   /* In the corresponding Pin Change Mask Register: indicate
                             * which pin(s) of that port activate the ISR. */
                                
    sei();
    startTimer();

    while (seconds < 30) { // Wait for 30 seconds or button press to start
        uint16_t potentio = readPotentio();
        writeNumber(potentio);

        if (buttonIsPressed) {
            stopTimer();
            break;
        }
    }
    stopTimer();

    srand(readPotentio()); // seed the random number generator with the value of the potentiometer

    currentPlayer = (rand() % 2 == 0) ? 'P' : 'C'; // Decide who starts randomly
    for (int i = 0; i < 500; i++) {
        displayGameStatus();
    }
    while (1) {
        if (currentPlayer == 'P') {
            handlePlayerTurn();
        } else {
            handleComputerTurn();
        }
    }

    return 0;
}
