// Шаг 3: кнопка без дребезга.
// Кнопка подключена между пином 9 и GND.
// Нажал кнопку: загорается светодиод L на плате, в Serial Monitor пишется «нажата».
#include <Arduino.h>

const int BUTTON_PIN = 9;               // к какому пину подключена кнопка
const unsigned long DEBOUNCE_MS = 30;   // сколько миллисекунд сигнал должен быть ровным, чтобы ему поверить

int stableState = HIGH;          // состояние, которому мы поверили (HIGH = отпущена)
int lastRaw = HIGH;              // что пин показывал в прошлый раз, даже если это дребезг
unsigned long lastChangeMs = 0;  // когда пин последний раз дёрнулся
int pressCount = 0;              // сколько раз нажали

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // пин 9 читает сигнал, внутренний резистор тянет его к 5V
  pinMode(LED_BUILTIN, OUTPUT);       // светодиод L на плате показывает нажатие
  Serial.begin(9600);
  Serial.println("Готово. Нажимай кнопку.");
}

void loop() {
  int raw = digitalRead(BUTTON_PIN);  // «сырое» значение пина, вместе с дребезгом

  // Пин дёрнулся? Запоминаем момент и начинаем отсчёт 30 мс заново
  if (raw != lastRaw) {
    lastChangeMs = millis();  // millis() = сколько миллисекунд прошло с включения платы
    lastRaw = raw;
  }

  // Сигнал держится ровно уже 30 мс и отличается от того, чему мы верили? Значит, это настоящее нажатие
  if (millis() - lastChangeMs >= DEBOUNCE_MS && raw != stableState) {
    stableState = raw;

    if (stableState == LOW) {
      pressCount++;
      Serial.print("нажата, раз: ");
      Serial.println(pressCount);
    } else {
      Serial.println("отпущена");
    }
  }

  // Светодиод горит, пока кнопка зажата
  digitalWrite(LED_BUILTIN, stableState == LOW ? HIGH : LOW);
}
