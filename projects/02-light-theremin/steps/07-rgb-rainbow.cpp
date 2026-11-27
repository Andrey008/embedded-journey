// Step 7: RGB LED module K851264.
// Three PWM pins drive red, green and blue. Mixing their brightness gives any color.
// Demo: 7 fixed colors (1 s each), then a smooth rainbow (6 s), repeat.
#include <Arduino.h>

// PWM pins not used by tone() (3 and 11 are).
// Found by test: pin 9 lights green, pin 6 blue, pin 5 red.
const int PIN_R = 5;
const int PIN_G = 9;
const int PIN_B = 6;

// true  = common anode (common leg on 5V): 255 means OFF, so values are inverted
// false = common cathode (common leg on GND): 255 means fully ON
const bool COMMON_ANODE = false;

struct Color { const char* name; int r, g, b; };
const Color COLORS[] = {
  {"red",     255,   0,   0},
  {"green",     0, 255,   0},
  {"blue",      0,   0, 255},
  {"yellow",  255, 255,   0},         // red + green
  {"cyan",      0, 255, 255},         // green + blue
  {"magenta", 255,   0, 255},         // red + blue
  {"white",   255, 255, 255},         // all three
};
const int COLOR_COUNT = 7;
const unsigned long STEP_MS = 1000;            // each fixed color shows for 1 s
const unsigned long RAINBOW_MS = 6000;         // one full rainbow
const unsigned long CYCLE_MS = COLOR_COUNT * STEP_MS + RAINBOW_MS;

int lastShown = -1;                   // which fixed color was printed last (-1 = none)
int curR = 0, curG = 0, curB = 0;     // current color, for Teleplot
unsigned long lastPlotMs = 0;
const unsigned long PLOT_MS = 50;     // send r/g/b to Teleplot 20 times per second

void setColor(int r, int g, int b) {
  curR = r; curG = g; curB = b;       // remember before inverting
  if (COMMON_ANODE) {                 // common anode: LED lights when the pin is LOW
    r = 255 - r; g = 255 - g; b = 255 - b;
  }
  analogWrite(PIN_R, r);
  analogWrite(PIN_G, g);
  analogWrite(PIN_B, b);
}

// Color wheel: h = 0..359. 0 = red, 120 = green, 240 = blue, back to red at 360.
void wheel(int h) {
  int x = (long)(h % 120) * 255 / 120;          // 0..255 inside each third of the circle
  if (h < 120)      setColor(255 - x, x, 0);    // red -> green
  else if (h < 240) setColor(0, 255 - x, x);    // green -> blue
  else              setColor(x, 0, 255 - x);    // blue -> red
}

void setup() {
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  unsigned long t = millis() % CYCLE_MS;        // time inside the current cycle

  if (t < COLOR_COUNT * STEP_MS) {
    int i = t / STEP_MS;                        // which fixed color now: 0..6
    setColor(COLORS[i].r, COLORS[i].g, COLORS[i].b);
    if (i != lastShown) {                       // print only when the color changes
      Serial.println(COLORS[i].name);
      lastShown = i;
    }
  } else {
    if (lastShown != -1) { Serial.println("rainbow"); lastShown = -1; }
    int h = (t - COLOR_COUNT * STEP_MS) * 360 / RAINBOW_MS;   // 0..359 over 6 s
    wheel(h);
  }

  if (millis() - lastPlotMs >= PLOT_MS) {       // Teleplot: three lines, one per channel
    lastPlotMs = millis();
    Serial.print(">r:"); Serial.println(curR);
    Serial.print(">g:"); Serial.println(curG);
    Serial.print(">b:"); Serial.println(curB);
  }
}
