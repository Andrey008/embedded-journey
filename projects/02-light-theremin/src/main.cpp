// Step 8: Light theremin.
// Hand over the photoresistor changes the buzzer pitch and the RGB color together.
// Start: 3 s calibration (wave your hand), the sketch learns your "dark" and "bright".
// Button on pin 2 mutes / unmutes the sound, the color keeps working.
#include <Arduino.h>

const int LIGHT_PIN = A1;             // divider: photoresistor on top, 10k below
const int BUZZER_PIN = 8;             // tone() uses Timer2 -> no PWM on 3 and 11
const int BUTTON_PIN = 2;
const int PIN_R = 5;                  // RGB module K851264, real leg order GND G B R
const int PIN_G = 9;
const int PIN_B = 6;

const unsigned long CALIBRATE_MS = 3000;
const unsigned long UPDATE_MS = 20;   // read light / update sound and color 50 times per second
const unsigned long PLOT_MS = 50;     // Teleplot 20 times per second
const unsigned long DEBOUNCE_MS = 30;
const int FREQ_LOW = 200;             // Hz when dark (hand close)
const int FREQ_HIGH = 2000;           // Hz when bright (hand away)

int darkLevel = 1023;                 // calibration result: lowest A1 seen
int brightLevel = 0;                  // calibration result: highest A1 seen
float smooth = 0;                     // filtered light value (less jitter)
bool soundOn = true;
unsigned long lastUpdateMs = 0;
unsigned long lastPlotMs = 0;
int pos = 0;                          // hand position 0 (dark) .. 1000 (bright)
int freq = 0;

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

void setColor(int r, int g, int b) {  // common cathode: 255 = full brightness
  analogWrite(PIN_R, r);
  analogWrite(PIN_G, g);
  analogWrite(PIN_B, b);
}

// Color wheel: h = 0..240. 240 = blue, 120 = green, 0 = red.
void wheel(int h) {
  int x = (long)(h % 120) * 255 / 120;
  if (h >= 240)     setColor(0, 0, 255);       // exactly blue (240 % 120 would give 0)
  else if (h < 120) setColor(255 - x, x, 0);   // red -> green
  else              setColor(0, 255 - x, x);   // green -> blue
}

void calibrate() {
  Serial.println("Calibrating 3 s: wave your hand over the sensor");
  unsigned long start = millis();
  while (millis() - start < CALIBRATE_MS) {    // the only blocking part, before the music starts
    int v = analogRead(LIGHT_PIN);
    if (v < darkLevel) darkLevel = v;          // remember the darkest
    if (v > brightLevel) brightLevel = v;      // remember the brightest
    int blink = (millis() / 250) % 2;          // white blink 2 times per second = "calibrating"
    setColor(blink * 60, blink * 60, blink * 60);
  }
  if (brightLevel - darkLevel < 100) {         // hand was not waved: use numbers from step 6
    darkLevel = 106;
    brightLevel = 726;
  }
  Serial.print("dark = ");
  Serial.print(darkLevel);
  Serial.print(", bright = ");
  Serial.println(brightLevel);
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  Serial.begin(115200);
  calibrate();
  smooth = analogRead(LIGHT_PIN);
}

void loop() {
  if (buttonPressed()) {
    soundOn = !soundOn;
    if (!soundOn) noTone(BUZZER_PIN);
  }

  if (millis() - lastUpdateMs >= UPDATE_MS) {
    lastUpdateMs = millis();
    int raw = analogRead(LIGHT_PIN);
    smooth += (raw - smooth) * 0.3;            // low-pass filter: move 30% toward the new value

    pos = map((int)smooth, darkLevel, brightLevel, 0, 1000);  // your range -> 0..1000
    pos = constrain(pos, 0, 1000);                            // stay inside if you go beyond

    freq = FREQ_LOW + (long)pos * (FREQ_HIGH - FREQ_LOW) / 1000;
    if (soundOn) tone(BUZZER_PIN, freq);

    wheel(240 - (long)pos * 240 / 1000);       // dark = blue (240), bright = red (0)
  }

  if (millis() - lastPlotMs >= PLOT_MS) {
    lastPlotMs = millis();
    Serial.print(">light:"); Serial.println((int)smooth);
    Serial.print(">pos:");   Serial.println(pos);
    Serial.print(">freq:");  Serial.println(soundOn ? freq : 0);
  }
}
