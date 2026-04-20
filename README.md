# 99Game

An embedded implementation of the "99" dice game on an Arduino Uno (ATmega328P), written in C and built with PlatformIO. First to reach **99** points wins — played between a human and a rule-based computer opponent on a 7-segment display with physical buttons, LEDs, a potentiometer, and a buzzer.

Final project for the course **ACS 105 (Embedded Systems / Computer Architecture)**.

---

## Game rules

On each turn the active player rolls a virtual die (1–6) as many times as they want. Two special rules shape the strategy:

- Rolling a **1** wipes the turn score and ends the turn.
- At the end of a turn the player may either **(A) add** their accumulated turn score to their total, or **(S) subtract** that amount from the opponent's total.

First player to **99** wins. The computer's decisions are driven by thresholds:

| Turn score | Computer behavior |
|-----------:|-------------------|
| `< 10` | keep rolling |
| `10–15` | roll again with 50% probability |
| `> 15` | stop |
| `> 80` opponent score | 80% chance to pick the subtract action |

---

## Hardware

| Component | Role |
|-----------|------|
| Arduino Uno (ATmega328P) | Main MCU |
| 7-segment 4-digit display | Shows current player (`P`/`C`), turn score, total score, and roll value |
| 3 push-buttons | Roll (BTN1), confirm / add (BTN2), subtract-from-opponent (BTN3) |
| Potentiometer | Seeds `rand()` via `srand(readPotentio())` and doubles as the pre-game "start" input |
| LEDs | Flash during score-subtraction animations |
| Buzzer | Tone on turn-end (roll of 1) and on win |
| USART | Serial log of every turn for post-game review |

---

## Repository layout

```
99Game/
├── platformio.ini         # Uno + atmelavr configuration
├── src/
│   └── main.c             # Stub entry; the real game lives under exercises/final_project
├── include/               # Project-wide headers
├── lib/                   # Hand-written driver libraries (one per peripheral)
│   ├── button/            # Debounced button read via pin-change interrupts
│   ├── buzzer/            # Tone generation
│   ├── display/           # 7-segment multiplexing (writeNumberToSegment, writeCharToSegment, ...)
│   ├── led/               # enableAllLeds / lightUpAllLeds
│   ├── potentiometer/     # ADC read
│   ├── timer/             # Timer2 CTC mode, used for 1 s tick via OCR2A
│   └── usart/             # Serial printf retarget
└── exercises/
    ├── week1/ … week4/    # Weekly demos, exercises, and mini-projects
    └── final_project/     # The full 99 game (project.c)
```

To run the final project on the board, copy `exercises/final_project/project.c` into `src/main.c` and build/upload with PlatformIO:

```bash
pio run --target upload
pio device monitor
```

Per-week code follows the same pattern — copy the relevant `projectX.c` into `src/main.c` before building.

---

## Implementation highlights

- **Pin-change interrupt debouncing** — `ISR(PCINT1_vect)` waits 250 µs and re-reads `PINC` before committing a button press.
- **Timer2 CTC tick** — `OCR2A = 249` with a /256 prescaler gives a 4 ms tick; `ISR(TIMER2_COMPA_vect)` counts 250 ticks to measure 1 s.
- **Dynamic turn log** — each turn (player, rolls, turn score, running totals, decision) is pushed into a dynamically `realloc`'d `GameTurn*` array and dumped over USART on win.
- **Non-deterministic seeding** — the potentiometer value at power-on seeds `rand()` so the computer's rolls differ every match.

---

## Author

Souhaib Othmani — ACS 105.
