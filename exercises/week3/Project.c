#include <util/delay.h>
#include <avr/io.h>
#include <usart.h>
#include <display.h>
#include <stdlib.h> // For rand()
#include <button.h>
#include <led.h>
#include <potentiometer.h>

#define START_NUMBER 21
#define MAX_NUMBER 3

int matches = START_NUMBER;
char currentPlayer = 'P'; // 'P' for Player, 'C' for Computer
int playerChoice = 1;
int seed = 0;

typedef struct {
    char player;
    int matchesBefore;
    int matchesTaken;
} GameMove;

GameMove* gameMoves = NULL;
int moveCount = 0;

// Function to display the current game status
void displayGameStatus() {
    writeNumberToSegment(2, matches / 10);
    writeNumberToSegment(3, matches % 10);
    writeCharToSegment(1, currentPlayer);
    writeNumberToSegment(0, playerChoice);
}

// Function to switch turns
void switchTurn() {
    playerChoice = 1;
    currentPlayer = (currentPlayer == 'P') ? 'C' : 'P';
    displayGameStatus();
}

// Function to store the current move
void storeMove(char player, int matchesBefore, int matchesTaken) {
    moveCount++;
    gameMoves = realloc(gameMoves, moveCount * sizeof(GameMove));
    gameMoves[moveCount - 1].player = player;
    gameMoves[moveCount - 1].matchesBefore = matchesBefore;
    gameMoves[moveCount - 1].matchesTaken = matchesTaken;
}

// Function to handle the player's turn
void handlePlayerTurn() {
    while (currentPlayer == 'P') {
        displayGameStatus();

        if (buttonPushed(1)) { // Decrease number of matches to take (minimum 1)
            _delay_ms(250); //debounce
            if (playerChoice > 1) {
                playerChoice--;
                displayGameStatus();
            }
        }

        if (buttonPushed(3)) { // Increase number of matches to take (maximum 3)
            _delay_ms(250); //debounce
            if (playerChoice < MAX_NUMBER) {
                playerChoice++;
                displayGameStatus();
            }
        }

        if (buttonPushed(2)) { // Confirm choice
            int matchesBefore = matches;
            matches -= playerChoice;
            storeMove('P', matchesBefore, playerChoice);
            if (matches <= 1) {
                writeStringAndWait("CWIN", 5000); // Display "CWIN" for 5 seconds
                break;
            }
            switchTurn();
        }
    }
}

// Function to handle the computer's turn
void handleComputerTurn() {
    while (currentPlayer == 'C') {
        for (int i = 0; i < 10000; i++) {
            displayGameStatus(); // display current turn status
        }

        int compChoice = (matches - 1) % (MAX_NUMBER + 1);
        if (compChoice == 0) {
            compChoice = (rand() % MAX_NUMBER) + 1;
        }
        playerChoice = compChoice;
        int matchesBefore = matches;
        matches -= compChoice;
        storeMove('C', matchesBefore, compChoice);

        for (int i = 0; i < 10000; i++) {
            displayGameStatus(); // display current turn status
        }

        if (matches <= 1) {
            writeStringAndWait("PWIN", 5000); // Display "PWIN" for 5 seconds
            break;
        }
        switchTurn();
    }
}

// Function to print game progress
void printGameProgress() {
    for (int i = 0; i < moveCount; i++) {
        printf("Turn: %c, Matches before: %d, Matches taken: %d\r\n", 
               gameMoves[i].player, 
               gameMoves[i].matchesBefore, 
               gameMoves[i].matchesTaken);
    }
}

int main() {
    initUSART();
    initDisplay();
    enableAllButtons();
    enablePotentio();

    while (1) {
        uint16_t potentio = readPotentio();
        writeNumberAndWait(potentio, 100);

        if (buttonPushed(1)) {
            break;
        }
    }

    seed = readPotentio(); // Seed the random number generator with the value of the potentiometer
    srand(seed);

    currentPlayer = (rand() % 2 == 0) ? 'P' : 'C'; // Decide who starts randomly
    displayGameStatus();

    while (matches > 1) {
        if (currentPlayer == 'P') {
            handlePlayerTurn();
        } else {
            handleComputerTurn();
        }
    }

    // Print game progress on the serial monitor
    printGameProgress();

    free(gameMoves); // Free dynamically allocated memory

    return 0;
}
