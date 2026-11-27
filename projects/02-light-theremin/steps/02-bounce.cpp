// Step 2: button bounce.
// No debounce on purpose: we record the time of every raw HIGH/LOW change of the pin.
// One clean press = 2 changes (down + up). Anything above that is bounce.
// Changes are split into two groups: "press" (around going down) and
// "release" (around going up), so we can see which one bounces and for how long.
#include <Arduino.h>

const int BUTTON_PIN = 2;
const unsigned long QUIET_MS = 200;     // no changes for this long = press is over
const unsigned long GROUP_GAP_US = 10000; // gap > 10 ms between changes = new group (press -> release)
const int MAX_EDGES = 64;               // how many change times we keep per press

unsigned long edgeUs[MAX_EDGES];  // micros() of each change
int edges = 0;                    // number of changes in the current press
int lastRaw = HIGH;               // pin state on the previous loop
unsigned long lastChangeMs = 0;   // when the pin last changed
int pressCount = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // pin pulled to 5V inside, button pulls it to GND
  Serial.begin(9600);
  Serial.println("Press the button. Watch the scope.");
}

void printReport() {
  int stored = min(edges, MAX_EDGES);

  // Find where the "press" group ends: first big gap between two changes
  int split = stored;
  for (int i = 1; i < stored; i++) {
    if (edgeUs[i] - edgeUs[i - 1] > GROUP_GAP_US) { split = i; break; }
  }

  pressCount++;
  Serial.print("Press #");
  Serial.print(pressCount);
  Serial.print(": ");
  Serial.print(edges);
  Serial.println(" raw changes");

  // Press group: changes 0 .. split-1
  Serial.print("  press:   ");
  Serial.print(split);
  Serial.print(" changes, bounce ");
  Serial.print(edgeUs[split - 1] - edgeUs[0]);
  Serial.println(" us");

  // Release group: changes split .. stored-1
  if (split < stored) {
    Serial.print("  release: ");
    Serial.print(stored - split);
    Serial.print(" changes, bounce ");
    Serial.print(edgeUs[stored - 1] - edgeUs[split]);
    Serial.println(" us");
  }
}

void loop() {
  int raw = digitalRead(BUTTON_PIN);
  if (raw != lastRaw) {                          // pin changed
    if (edges < MAX_EDGES) edgeUs[edges] = micros();  // remember exact time (4 us steps)
    edges++;
    lastChangeMs = millis();
    lastRaw = raw;
  }

  // Button released and quiet for a while: print the result for this press
  if (edges > 0 && raw == HIGH && millis() - lastChangeMs > QUIET_MS) {
    printReport();
    edges = 0;
  }
}
