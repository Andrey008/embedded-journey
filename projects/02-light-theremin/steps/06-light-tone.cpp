// Step 6: photoresistor + passive buzzer.
// Photoresistor replaces the top resistor of the divider, so A1 follows the light.
// More light -> higher A1 -> higher tone on the buzzer (pin 8).
// The button on pin 2 mutes / unmutes the sound.
#include <Arduino.h>

const int LIGHT_PIN = A1;             // middle of the divider: photoresistor on top, 10k below
const int POT_PIN = A0;               // step 4: potentiometer still drives the LED
const int LED_PIN = 10;               // Timer1 PWM, not affected by tone()
const int BUZZER_PIN = 8;
const int BUTTON_PIN = 2;

const int FREQ_MIN = 200;             // Hz in full dark
const int FREQ_MAX = 2000;            // Hz in full light
const unsigned long UPDATE_MS = 50;   // read light and update the tone 20 times per second
const unsigned long DEBOUNCE_MS = 30;

bool soundOn = true;
unsigned long lastUpdateMs = 0;

int lastRaw = HIGH;                   // debounce, same as step 3
int stableState = HIGH;
unsigned long lastRawChangeMs = 0;

bool buttonPressed() {
  int raw = digitalRead(BUTTON_PIN);
  if (raw != lastRaw) {
    lastRaw = raw;
    lastRawChangeMs = millis();
  }
  if (millis() - lastRawChangeMs > DEBOUNCE_MS && raw != stableState) {
    stableState = raw;
    return stableState == LOW;
  }
  return false;
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);  // Teleplot default speed
}

void loop() {
  analogWrite(LED_PIN, analogRead(POT_PIN) / 4);

  if (buttonPressed()) {
    soundOn = !soundOn;               // flip: on -> off, off -> on
    if (!soundOn) noTone(BUZZER_PIN); // stop the square wave on pin 8
  }

  if (millis() - lastUpdateMs >= UPDATE_MS) {
    lastUpdateMs = millis();
    int light = analogRead(LIGHT_PIN);                     // 0 = dark, 1023 = bright
    int freq = map(light, 0, 1023, FREQ_MIN, FREQ_MAX);    // stretch 0..1023 to 200..2000

    if (soundOn) tone(BUZZER_PIN, freq);  // square wave with this frequency, runs in background

    Serial.print(">light:");
    Serial.println(light);
    Serial.print(">freq:");
    Serial.println(soundOn ? freq : 0);
  }
}
