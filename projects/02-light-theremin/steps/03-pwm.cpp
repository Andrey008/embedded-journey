// Step 3: PWM.
// LED on pin 10 "breathes" (brightness 0 -> 255 -> 0).
// The button on pin 2 switches modes: breathe -> 64 -> 128 -> 192 -> breathe.
// Fixed values are for looking at the PWM signal on the scope.
#include <Arduino.h>

const int LED_PIN = 10;               // PWM pin (marked ~10), about 490 Hz
const int BUTTON_PIN = 2;
const unsigned long DEBOUNCE_MS = 30; // step 2: worst bounce was ~2.3 ms
const unsigned long STEP_MS = 8;      // breathing speed: one brightness step every 8 ms

const int FIXED[] = {64, 128, 192};   // 25%, 50%, 75% duty
int mode = 0;                         // 0 = breathe, 1..3 = FIXED[mode - 1]

int brightness = 0;                   // current PWM value, 0..255
int dir = 1;                          // +1 = getting brighter, -1 = getting dimmer
unsigned long lastStepMs = 0;

int lastRaw = HIGH;                   // debounce: raw pin state on the previous loop
int stableState = HIGH;               // debounce: accepted button state
unsigned long lastRawChangeMs = 0;

// Returns true once per press (after the signal has been stable for DEBOUNCE_MS)
bool buttonPressed() {
  int raw = digitalRead(BUTTON_PIN);
  if (raw != lastRaw) {               // pin moved (maybe bounce): restart the timer
    lastRaw = raw;
    lastRawChangeMs = millis();
  }
  if (millis() - lastRawChangeMs > DEBOUNCE_MS && raw != stableState) {
    stableState = raw;                // stable long enough: accept the new state
    return stableState == LOW;        // LOW = pressed (INPUT_PULLUP)
  }
  return false;
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Mode: breathe. Press the button to switch.");
}

void loop() {
  if (buttonPressed()) {
    mode = (mode + 1) % 4;            // 0,1,2,3,0,1...
    if (mode == 0) {
      Serial.println("Mode: breathe");
    } else {
      int value = FIXED[mode - 1];
      analogWrite(LED_PIN, value);    // fixed duty: pin is HIGH value/255 of each period
      Serial.print("Mode: fixed ");
      Serial.print(value);
      Serial.print(" = ");
      Serial.print(value * 100L / 255);  // 100L: do the math in long, int would overflow
      Serial.println("% duty");
    }
  }

  if (mode == 0 && millis() - lastStepMs >= STEP_MS) {
    lastStepMs = millis();
    brightness += dir;
    if (brightness >= 255 || brightness <= 0) dir = -dir;  // turn around at the ends
    analogWrite(LED_PIN, brightness);
    if (brightness % 32 == 0) Serial.println(brightness);  // print every 32nd value only
  }
}
