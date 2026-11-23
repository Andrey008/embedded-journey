// Traffic light with a pedestrian button and a countdown display.
//
// Cycle: GREEN (wait for button, at least 5 s) → GREEN blinks 3 times → YELLOW 2 s
//        → RED + countdown 9..0 → RED+YELLOW 1 s → GREEN
// No delay(): loop() runs thousands of times per second and checks millis() every time.
#include <Arduino.h>

// ---------- pins ----------
const int RED_PIN = 10;
const int YELLOW_PIN = 11;
const int GREEN_PIN = 12;
const int BUTTON_PIN = 9;
const int SEGMENT_PINS[7] = {2, 3, 4, 5, 6, 7, 8};  // segments a, b, c, d, e, f, g

// ---------- timing, ms ----------
const unsigned long MIN_GREEN_MS = 5000;     // green stays on at least this long
const int BLINK_COUNT = 3;                   // how many times green blinks
const unsigned long BLINK_PERIOD_MS = 1000;  // one blink: half on, half off
const unsigned long YELLOW_MS = 2000;
const unsigned long COUNT_STEP_MS = 1000;    // countdown: one digit per second
const unsigned long RED_YELLOW_MS = 1000;
const unsigned long DEBOUNCE_MS = 30;

// ---------- digit patterns (same as step 4) ----------
//                            gfedcba
const byte DIGITS[10] = {
  0b0111111, 0b0000110, 0b1011011, 0b1001111, 0b1100110,
  0b1101101, 0b1111101, 0b0000111, 0b1111111, 0b1101111,
};

// ---------- traffic light state ----------
enum State { GREEN, GREEN_BLINK, YELLOW, RED, RED_YELLOW };

State state = GREEN;
unsigned long stateStartMs = 0;  // when the current state was entered
bool pedestrianWaiting = false;  // button pressed, pedestrian is waiting
int secondsLeft = 0;             // digit shown during the countdown
unsigned long lastCountMs = 0;   // when the countdown last ticked

// ---------- debounced button (same as step 3) ----------
int stableButton = HIGH;
int lastRawButton = HIGH;
unsigned long lastButtonChangeMs = 0;

// Returns true once, at the moment of a press. Holding and bounce don't count
bool buttonPressed() {
  int raw = digitalRead(BUTTON_PIN);
  if (raw != lastRawButton) {
    lastButtonChangeMs = millis();
    lastRawButton = raw;
  }
  if (millis() - lastButtonChangeMs >= DEBOUNCE_MS && raw != stableButton) {
    stableButton = raw;
    return stableButton == LOW;  // LOW = pressed (button shorts the pin to GND)
  }
  return false;
}

// ---------- helpers ----------
void setLights(bool red, bool yellow, bool green) {
  digitalWrite(RED_PIN, red);
  digitalWrite(YELLOW_PIN, yellow);
  digitalWrite(GREEN_PIN, green);
}

void showDigit(int n) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(SEGMENT_PINS[i], (DIGITS[n] >> i) & 1);  // bit i: is segment i lit?
  }
}

void clearDisplay() {
  for (int i = 0; i < 7; i++) digitalWrite(SEGMENT_PINS[i], LOW);
}

// Enter a new state: set the lights and remember the start time
void enterState(State s) {
  state = s;
  stateStartMs = millis();

  switch (s) {
    case GREEN:
      setLights(false, false, true);
      clearDisplay();
      Serial.println("GREEN: cars go");
      break;
    case GREEN_BLINK:
      setLights(false, false, true);
      Serial.println("GREEN BLINKING");
      break;
    case YELLOW:
      setLights(false, true, false);
      Serial.println("YELLOW");
      break;
    case RED:
      setLights(true, false, false);
      secondsLeft = 9;
      showDigit(secondsLeft);
      lastCountMs = millis();
      pedestrianWaiting = false;
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("RED: pedestrians cross");
      break;
    case RED_YELLOW:
      setLights(true, true, false);
      clearDisplay();
      Serial.println("RED + YELLOW: get ready");
      break;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  for (int i = 0; i < 7; i++) pinMode(SEGMENT_PINS[i], OUTPUT);

  enterState(GREEN);
}

void loop() {
  // 1. Always check the button, in any state. This is why we avoid delay()
  if (buttonPressed() && state == GREEN && !pedestrianWaiting) {
    pedestrianWaiting = true;
    digitalWrite(LED_BUILTIN, HIGH);  // on-board LED: request accepted, please wait
    Serial.println("button: pedestrian waiting");
  }

  // 2. Time spent in the current state
  unsigned long inState = millis() - stateStartMs;

  // 3. Decide whether to move on
  switch (state) {
    case GREEN:
      if (pedestrianWaiting && inState >= MIN_GREEN_MS) enterState(GREEN_BLINK);
      break;

    case GREEN_BLINK:
      if (inState >= BLINK_COUNT * BLINK_PERIOD_MS) {
        enterState(YELLOW);
      } else {
        // Position inside the current blink: first half on, second half off
        bool on = (inState % BLINK_PERIOD_MS) < BLINK_PERIOD_MS / 2;
        digitalWrite(GREEN_PIN, on);
      }
      break;

    case YELLOW:
      if (inState >= YELLOW_MS) enterState(RED);
      break;

    case RED:
      if (millis() - lastCountMs >= COUNT_STEP_MS) {
        lastCountMs = millis();
        if (secondsLeft == 0) {
          enterState(RED_YELLOW);  // zero was shown for a full second, countdown done
        } else {
          secondsLeft--;
          showDigit(secondsLeft);
          Serial.println(secondsLeft);
        }
      }
      break;

    case RED_YELLOW:
      if (inState >= RED_YELLOW_MS) enterState(GREEN);
      break;
  }
}
