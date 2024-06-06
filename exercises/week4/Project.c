#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <display.h>
#include <usart.h>
#include <time.h>
#include <led.h>
#include <string.h>
#include <ctype.h>
#include <button.h>
#include <buzzer.h>

// #define DURATION 150

int currentSpeed = 100; // speed in m/s (meters per second)
float gravity = 1.622;  // acceleration in m/s²
int distance = 9999;    // distance to the lunar surface in meters - m
int fuelReserve = 1500; // litre
int arrlen = 0;

#define MULTIPLE 250
#define DURATION 500

typedef struct
{
    int length;
    int *burst;
    int *distance;
    float gravity;
    int *fuelReserve;
    int *current_speed;

} LUNDER_LANDER;

LUNDER_LANDER *lunar_lander;

int counter;
int seconds;
int minutes;
int loop;
int burst;
int burst_duration;

//FREQUENCIES OF THE NOTES
#define C5 523.250
#define D5 587.330
#define E5 659.250
#define F5 698.460
#define G5 783.990
#define A5 880.00
#define B5 987.770
#define C6 1046.500


void printLunderLanderData(LUNDER_LANDER *lunar_lander)
{

    printf("Current Speed\n");
    for (int i = 0; i < lunar_lander->length; i++)
    {
        printf("%d ", lunar_lander->current_speed[i]);
    }
    printf("\nDistance\n");
    for (int i = 0; i < lunar_lander->length; i++)
    {
        printf("%d ", lunar_lander->distance[i]);
    }
    printf("\nFuel Reserve\n");
    for (int i = 0; i < lunar_lander->length; i++)
    {
        printf("%d ", lunar_lander->fuelReserve[i]);
    }
    printf("\n");
}

uint8_t crashed()
{
    printf("Ow, You have crashed !!\n");

    uint32_t frequencies[] = {C5, D5, D5, C6};
    for (int i = 0; i < 4; i++)
    {
        enableBuzzer();
        playTone(frequencies[i], DURATION);
    }
    printLunderLanderData(lunar_lander);
  loop=0;
    return loop;
}

uint8_t safe_landing()
{
    printf("Congratulations, You have safe landed !!\n");
    uint32_t frequencies[] = {C5, D5, E5, F5, G5, A5, B5, C6};
    for (int i = 0; i < 8; i++)
    {
        enableBuzzer();
        playTone(frequencies[i], DURATION);
    }
    printLunderLanderData(lunar_lander);
    loop=0;
    return loop;
}

void recalculate_newSituation(int *currentSpeed, float *gravity, int *burst, int *distance, int *fuelReserve, LUNDER_LANDER *lunar_lander)
{
    *burst = burst_duration;
    if (*burst > 50)
    {
        *burst = 50;
    }
    *currentSpeed += (*gravity - *burst) / 5;
    *distance -= *currentSpeed;
    *fuelReserve -= *burst;

    lunar_lander->current_speed[lunar_lander->length] = *currentSpeed;
    lunar_lander->distance[lunar_lander->length] = *distance;
    lunar_lander->fuelReserve[lunar_lander->length] = *fuelReserve;
}

void tick()
{
    if (seconds == 60)
    {
        seconds = 0;
        minutes += 1;
    }
}

void showParameters(int *currentSpeed, int *current_fuelReserve, int *current_distance)
{
    uint8_t fuel_tank = (*current_fuelReserve / 375);
    writeNumber(*current_distance);
    enableAllLeds();
    if (fuel_tank == 4)
    {
        lightUpAllLeds();
    }
    else
    {
        lightDownAllLeds();
        for (uint8_t i = 0; i < fuel_tank; i++)
        {
            lightUpOneLed(i);
        }
    }
    if ((*current_distance <= 3) && *currentSpeed <= 5)
    {
        safe_landing();
    }
    else if ((*current_distance <= 3 && *currentSpeed > 5) || (*current_distance > 3 && *current_fuelReserve <= 0))
    {
        crashed();
    }
}

void initTimer() {
    OCR2A = 249;
    TCCR2A |= _BV(WGM01); //enable CTC mode
    TIMSK2 |= _BV(OCIE2A); //enable OCRA(interrupt)
}

void startTimer() {
    TCCR2B |= _BV(CS22) | _BV(CS21); //set factor as 256  
}

ISR(TIMER2_COMPA_vect)
{
  counter++;
  if ((counter + 1) % MULTIPLE == 0)
  {
    tick();
    seconds += 1;
    if (seconds % 1 == 0)
    {
      recalculate_newSituation(&currentSpeed, &gravity, &burst, &distance, &fuelReserve, lunar_lander);
      printf("&currentSpeed %d, &gravity %d, &burst %d, &distance %d, &fuelReserve %d, lunar_lander %d\n",
                                &currentSpeed, &gravity, &burst, &distance, &fuelReserve, lunar_lander);
      lunar_lander->length += 1;
    }
  }
}

ISR(PCINT1_vect)
{
  if (bit_is_clear(PINC , PC2))
  {
    burst_duration += 1;
  }
}

void initADC(){
        PCICR |= _BV(PCIE1);  /* in Pin Change Interrupt Control Register: indicate
                             * which interrupt(s) you want to activate (PCIE0: port B,
                             * PCIE1: port C, PCIE2: port D) */

        PCMSK1 |= _BV(PC2);   /* In the corresponding Pin Change Mask Register: indicate
                             * which pin(s) of that port activate the ISR. */
}

int main()
{
  initADC();
  initUSART();
  initTimer();
  initButton();
  initDisplay();
  enableBTNinterrupt();
  lunar_lander = malloc(sizeof(LUNDER_LANDER));
  lunar_lander->burst = malloc(sizeof(int));
  lunar_lander->distance = malloc(sizeof(int));
  lunar_lander->fuelReserve = malloc(sizeof(int));
  lunar_lander->distance = malloc(sizeof(int));

  lunar_lander->length = 0;
  burst_duration = 0;
  sei();
  loop=1;
  while (loop)
  {
    startTimer();
    showParameters(&currentSpeed, &fuelReserve, &distance);
  }
  cli();
  free(lunar_lander->burst);
  free(lunar_lander->current_speed);
  free(lunar_lander->fuelReserve);
  free(lunar_lander->distance);
  return 0;
}
