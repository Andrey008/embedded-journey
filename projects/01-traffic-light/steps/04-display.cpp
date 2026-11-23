// Step 4: 5161AS 7-segment display, cycling digits 0–9.
// Segments a–g on pins 2–8 through 330 Ω resistors, COM to GND.
#include <Arduino.h>

// Uno pin for each segment, in order a, b, c, d, e, f, g
const int SEGMENT_PINS[7] = {2, 3, 4, 5, 6, 7, 8};
const char SEGMENT_NAMES[7] = {'a', 'b', 'c', 'd', 'e', 'f', 'g'};

// Segment pattern for each digit.
// Read right to left: lowest bit is segment a, highest is g. 1 = on, 0 = off.
//                            gfedcba
const byte DIGITS[10] = {
  0b0111111,  // 0: all but g
  0b0000110,  // 1: b, c
  0b1011011,  // 2: a, b, d, e, g
  0b1001111,  // 3: a, b, c, d, g
  0b1100110,  // 4: b, c, f, g
  0b1101101,  // 5: a, c, d, f, g
  0b1111101,  // 6: all but b
  0b0000111,  // 7: a, b, c
  0b1111111,  // 8: all seven
  0b1101111,  // 9: all but e
};

const unsigned long STEP_MS = 1000;  // how often the digit changes
unsigned long lastStepMs = 0;
int digit = 0;

// Show digit n: set each of the 7 segments on or off
void showDigit(int n) {
  for (int i = 0; i < 7; i++) {
    bool on = DIGITS[n] & (1 << i);  // bit i: is segment i lit?
    digitalWrite(SEGMENT_PINS[i], on ? HIGH : LOW);
  }
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) {
    pinMode(SEGMENT_PINS[i], OUTPUT);
  }

  // Wiring self-test: light segments one by one (a → g) and print which one should be on.
  // If a different segment lights up, the wires are swapped. delay() is fine here: one-time check before start.
  Serial.println("Self-test: segments one by one");
  for (int i = 0; i < 7; i++) {
    Serial.print("segment ");
    Serial.println(SEGMENT_NAMES[i]);
    digitalWrite(SEGMENT_PINS[i], HIGH);
    delay(500);
    digitalWrite(SEGMENT_PINS[i], LOW);
  }

  Serial.println("Digits 0-9");
  showDigit(digit);
  lastStepMs = millis();
}

void loop() {
  // One second passed: next digit. Non-blocking, same millis() pattern as the debounce
  if (millis() - lastStepMs >= STEP_MS) {
    lastStepMs = millis();
    digit = (digit + 1) % 10;  // wrap around after 9
    showDigit(digit);
    Serial.println(digit);
  }
}
