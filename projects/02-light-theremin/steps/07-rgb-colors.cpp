// Step 7: RGB LED module K851264 (common cathode: GND R G B).
// Cycle: OFF for 3 s (marks the start), then 7 colors for 1 s each, repeat.
#include <Arduino.h>

// PWM pins not used by tone() (3 and 11 are).
// Found by test: pin 9 lights green, pin 6 blue, pin 5 red.
const int PIN_R = 5;
const int PIN_G = 9;
const int PIN_B = 6;

const unsigned long OFF_MS = 3000;    // pause at the start of every cycle
const unsigned long COLOR_MS = 1000;  // each color shows for 1 s

struct Color { const char* name; int r, g, b; };
const Color COLORS[] = {
  {"1 red",     255,   0,   0},
  {"2 green",     0, 255,   0},
  {"3 blue",      0,   0, 255},
  {"4 yellow",  255, 255,   0},       // red + green
  {"5 cyan",      0, 255, 255},       // green + blue
  {"6 magenta", 255,   0, 255},       // red + blue
  {"7 white",   255, 255, 255},       // all three
};
const int COLOR_COUNT = 7;
const unsigned long CYCLE_MS = OFF_MS + COLOR_COUNT * COLOR_MS;   // 3 + 7 = 10 s

int shown = -2;                       // what is on now: -1 = off, 0..6 = color (-2 = nothing yet)

void setColor(int r, int g, int b) {
  analogWrite(PIN_R, r);              // common cathode: 255 = full brightness
  analogWrite(PIN_G, g);
  analogWrite(PIN_B, b);
}

void setup() {
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  unsigned long t = millis() % CYCLE_MS;              // time inside the 10 s cycle

  int want;                                           // what should be on right now
  if (t < OFF_MS) want = -1;                          // first 3 s: off
  else want = (t - OFF_MS) / COLOR_MS;                // then 0..6, one per second

  if (want != shown) {                                // changed: update LED and print once
    shown = want;
    if (want == -1) {
      setColor(0, 0, 0);
      Serial.println("--- off (3 s), next: red ---");
    } else {
      setColor(COLORS[want].r, COLORS[want].g, COLORS[want].b);
      Serial.println(COLORS[want].name);
    }
  }
}
