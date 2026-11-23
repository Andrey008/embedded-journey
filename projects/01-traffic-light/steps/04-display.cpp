// Шаг 4: индикатор 5161AS, цифры 0–9 по кругу.
// Сборка по таблице в EMB-1: палочки a–g на пинах 2–8 через резисторы 330 Ом, COM на GND.
#include <Arduino.h>

// Какой пин Uno управляет какой палочкой. Порядок: a, b, c, d, e, f, g
const int SEGMENT_PINS[7] = {2, 3, 4, 5, 6, 7, 8};
const char SEGMENT_NAMES[7] = {'a', 'b', 'c', 'd', 'e', 'f', 'g'};

// Какие палочки горят для каждой цифры.
// Читать справа налево: последний бит — палочка a, первый — g. 1 = горит, 0 = не горит.
//                            gfedcba
const byte DIGITS[10] = {
  0b0111111,  // 0: все, кроме g
  0b0000110,  // 1: b, c
  0b1011011,  // 2: a, b, d, e, g
  0b1001111,  // 3: a, b, c, d, g
  0b1100110,  // 4: b, c, f, g
  0b1101101,  // 5: a, c, d, f, g
  0b1111101,  // 6: все, кроме b
  0b0000111,  // 7: a, b, c
  0b1111111,  // 8: все семь
  0b1101111,  // 9: все, кроме e
};

const unsigned long STEP_MS = 1000;  // как часто менять цифру
unsigned long lastStepMs = 0;        // когда последний раз меняли
int digit = 0;                       // какую цифру показываем сейчас

// Зажечь цифру n: пройтись по 7 палочкам и каждой сказать «гори» или «не гори»
void showDigit(int n) {
  for (int i = 0; i < 7; i++) {
    bool on = DIGITS[n] & (1 << i);  // достаём i-й бит: горит ли палочка номер i
    digitalWrite(SEGMENT_PINS[i], on ? HIGH : LOW);
  }
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) {
    pinMode(SEGMENT_PINS[i], OUTPUT);  // все 7 пинов работают как выходы
  }

  // Проверка сборки: зажигаем палочки по одной, a → g, и пишем в монитор, какая сейчас должна гореть.
  // Если горит не та, значит, провода на макетке перепутаны. delay() тут можно: это разовая проверка до старта.
  Serial.println("Проверка: палочки по одной");
  for (int i = 0; i < 7; i++) {
    Serial.print("горит ");
    Serial.println(SEGMENT_NAMES[i]);
    digitalWrite(SEGMENT_PINS[i], HIGH);
    delay(500);
    digitalWrite(SEGMENT_PINS[i], LOW);
  }

  Serial.println("Цифры 0-9");
  showDigit(digit);
  lastStepMs = millis();
}

void loop() {
  // Прошла секунда? Переходим к следующей цифре. Без delay(), сверяясь с часами, как в антидребезге
  if (millis() - lastStepMs >= STEP_MS) {
    lastStepMs = millis();
    digit = (digit + 1) % 10;  // % 10 = остаток от деления: после 9 снова 0
    showDigit(digit);
    Serial.println(digit);
  }
}
