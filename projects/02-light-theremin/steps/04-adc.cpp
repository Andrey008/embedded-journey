// Step 4: ADC.
// Potentiometer on A0 sets the LED brightness on pin 10.
// Values are printed in Teleplot format (">name:value") so VS Code can draw a graph.
#include <Arduino.h>

const int POT_PIN = A0;               // middle leg of the potentiometer
const int LED_PIN = 10;               // PWM pin from step 3
const unsigned long PRINT_MS = 50;    // print 20 times per second (9600 baud can't do much more)

unsigned long lastPrintMs = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int raw = analogRead(POT_PIN);      // ADC: 0 V -> 0, 5 V -> 1023 (10 bits)
  int brightness = raw / 4;           // 0..1023 -> 0..255 for analogWrite
  analogWrite(LED_PIN, brightness);   // the knob now sets the PWM duty

  if (millis() - lastPrintMs >= PRINT_MS) {
    lastPrintMs = millis();
    float volts = raw * 5.0 / 1023;   // back to volts: each step is about 4.9 mV

    Serial.print(">pot:");            // Teleplot line: >name:value
    Serial.println(raw);
    Serial.print(">volts:");
    Serial.println(volts, 2);         // 2 digits after the dot
  }
}
