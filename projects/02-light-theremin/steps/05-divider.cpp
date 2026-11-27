// Step 5: voltage divider.
// Two 10k resistors between 5V and GND, the middle point goes to A1.
// Expected: 5 * 10 / (10 + 10) = 2.5 V, about 512 on the ADC.
// The potentiometer from step 4 still controls the LED.
#include <Arduino.h>

const int POT_PIN = A0;               // step 4: potentiometer
const int DIVIDER_PIN = A1;           // middle of the divider
const int LED_PIN = 10;
const unsigned long PRINT_MS = 100;   // 10 times per second

unsigned long lastPrintMs = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  analogWrite(LED_PIN, analogRead(POT_PIN) / 4);  // step 4 keeps working

  if (millis() - lastPrintMs >= PRINT_MS) {
    lastPrintMs = millis();
    int raw = analogRead(DIVIDER_PIN);   // 0..1023
    float volts = raw * 5.0 / 1023;      // assumes the board's 5V is exactly 5.00 V

    Serial.print(">divider:");
    Serial.println(raw);
    Serial.print(">divider_volts:");
    Serial.println(volts, 2);
  }
}
