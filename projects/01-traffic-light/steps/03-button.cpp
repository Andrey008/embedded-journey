// Step 3: debounced button.
// Button is wired between pin 9 and GND.
// Press it: the on-board LED (L) lights up and Serial Monitor prints "pressed".
#include <Arduino.h>

const int BUTTON_PIN = 9;
const unsigned long DEBOUNCE_MS = 30;  // signal must stay stable this long to be trusted

int stableState = HIGH;          // accepted state (HIGH = released)
int lastRaw = HIGH;              // last raw reading, bounce included
unsigned long lastChangeMs = 0;  // when the raw reading last changed
int pressCount = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // input with internal pull-up to 5V
  pinMode(LED_BUILTIN, OUTPUT);       // on-board LED shows the button state
  Serial.begin(9600);
  Serial.println("Ready. Press the button.");
}

void loop() {
  int raw = digitalRead(BUTTON_PIN);  // raw reading, including bounce

  // Reading changed: restart the 30 ms stability timer
  if (raw != lastRaw) {
    lastChangeMs = millis();  // milliseconds since the board started
    lastRaw = raw;
  }

  // Stable for 30 ms and different from the accepted state: a real press or release
  if (millis() - lastChangeMs >= DEBOUNCE_MS && raw != stableState) {
    stableState = raw;

    if (stableState == LOW) {
      pressCount++;
      Serial.print("pressed, count: ");
      Serial.println(pressCount);
    } else {
      Serial.println("released");
    }
  }

  // LED is on while the button is held
  digitalWrite(LED_BUILTIN, stableState == LOW ? HIGH : LOW);
}
