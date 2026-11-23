// Шаг 5: светофор с кнопкой пешехода и обратным отсчётом.
// Сборка по таблицам в EMB-1 (шаги 3, 4, 5).
//
// Цикл: ЗЕЛЁНЫЙ (ждём кнопку, минимум 5 с) → ЗЕЛЁНЫЙ МИГАЕТ 3 раза → ЖЁЛТЫЙ 2 с
//       → КРАСНЫЙ + отсчёт 9..0 → КРАСНЫЙ+ЖЁЛТЫЙ 1 с → ЗЕЛЁНЫЙ
// Никаких delay(): loop() крутится тысячи раз в секунду и каждый раз смотрит на часы millis().
#include <Arduino.h>

// ---------- пины ----------
const int RED_PIN = 10;
const int YELLOW_PIN = 11;
const int GREEN_PIN = 12;
const int BUTTON_PIN = 9;
const int SEGMENT_PINS[7] = {2, 3, 4, 5, 6, 7, 8};  // палочки a, b, c, d, e, f, g

// ---------- время, мс ----------
const unsigned long MIN_GREEN_MS = 5000;   // зелёный горит минимум столько
const int BLINK_COUNT = 3;                 // сколько раз мигает зелёный
const unsigned long BLINK_PERIOD_MS = 1000; // одно мигание: полсекунды горит, полсекунды нет
const unsigned long YELLOW_MS = 2000;
const unsigned long COUNT_STEP_MS = 1000;  // отсчёт: одна цифра в секунду
const unsigned long RED_YELLOW_MS = 1000;
const unsigned long DEBOUNCE_MS = 30;

// ---------- цифры для индикатора (как в шаге 4) ----------
//                            gfedcba
const byte DIGITS[10] = {
  0b0111111, 0b0000110, 0b1011011, 0b1001111, 0b1100110,
  0b1101101, 0b1111101, 0b0000111, 0b1111111, 0b1101111,
};

// ---------- состояние светофора ----------
// enum = список возможных состояний с понятными именами вместо чисел 0, 1, 2, 3
enum State { GREEN, GREEN_BLINK, YELLOW, RED, RED_YELLOW };

State state = GREEN;
unsigned long stateStartMs = 0;  // когда вошли в текущее состояние
bool pedestrianWaiting = false;  // пешеход нажал кнопку и ждёт
int secondsLeft = 0;             // что сейчас на индикаторе во время отсчёта
unsigned long lastCountMs = 0;   // когда последний раз уменьшали отсчёт

// ---------- кнопка с антидребезгом (как в шаге 3) ----------
int stableButton = HIGH;
int lastRawButton = HIGH;
unsigned long lastButtonChangeMs = 0;

// Возвращает true один раз, в момент нажатия. Удержание и дребезг не считаются
bool buttonPressed() {
  int raw = digitalRead(BUTTON_PIN);
  if (raw != lastRawButton) {
    lastButtonChangeMs = millis();
    lastRawButton = raw;
  }
  if (millis() - lastButtonChangeMs >= DEBOUNCE_MS && raw != stableButton) {
    stableButton = raw;
    return stableButton == LOW;  // LOW = нажата (кнопка замыкает пин на GND)
  }
  return false;
}

// ---------- помощники ----------
void setLights(bool red, bool yellow, bool green) {
  digitalWrite(RED_PIN, red);
  digitalWrite(YELLOW_PIN, yellow);
  digitalWrite(GREEN_PIN, green);
}

void showDigit(int n) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(SEGMENT_PINS[i], (DIGITS[n] >> i) & 1);  // i-й бит: горит ли палочка i
  }
}

void clearDisplay() {
  for (int i = 0; i < 7; i++) digitalWrite(SEGMENT_PINS[i], LOW);
}

// Переход в новое состояние: включаем нужные огни и засекаем время
void enterState(State s) {
  state = s;
  stateStartMs = millis();

  switch (s) {
    case GREEN:
      setLights(false, false, true);
      clearDisplay();
      Serial.println("ЗЕЛЁНЫЙ: машины едут");
      break;
    case GREEN_BLINK:
      setLights(false, false, true);
      Serial.println("ЗЕЛЁНЫЙ МИГАЕТ");
      break;
    case YELLOW:
      setLights(false, true, false);
      Serial.println("ЖЁЛТЫЙ");
      break;
    case RED:
      setLights(true, false, false);
      secondsLeft = 9;
      showDigit(secondsLeft);
      lastCountMs = millis();
      pedestrianWaiting = false;
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("КРАСНЫЙ: пешеходы идут");
      break;
    case RED_YELLOW:
      setLights(true, true, false);
      clearDisplay();
      Serial.println("КРАСНЫЙ + ЖЁЛТЫЙ: приготовиться");
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
  // 1. Кнопку проверяем всегда, в любом состоянии. Ради этого и нужен отказ от delay()
  if (buttonPressed() && state == GREEN && !pedestrianWaiting) {
    pedestrianWaiting = true;
    digitalWrite(LED_BUILTIN, HIGH);  // светодиод L на плате: «запрос принят, ждите»
    Serial.println("кнопка: пешеход ждёт");
  }

  // 2. Сколько времени мы уже в текущем состоянии
  unsigned long inState = millis() - stateStartMs;

  // 3. Решаем, пора ли переходить дальше
  switch (state) {
    case GREEN:
      if (pedestrianWaiting && inState >= MIN_GREEN_MS) enterState(GREEN_BLINK);
      break;

    case GREEN_BLINK:
      if (inState >= BLINK_COUNT * BLINK_PERIOD_MS) {
        enterState(YELLOW);
      } else {
        // Где мы внутри текущего мигания: первая половина секунды горит, вторая нет
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
          enterState(RED_YELLOW);  // ноль показали целую секунду, отсчёт окончен
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
