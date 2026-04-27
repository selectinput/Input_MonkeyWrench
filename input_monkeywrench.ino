#include <Keyboard.h>

struct KeyMap {
  uint8_t pin;
  uint8_t key;
  const char* label;
};

KeyMap keys[] = {
  { 2,  ' ',                  "Space" },
  { 3,  KEY_RETURN,           "Enter" },
  { 4,  KEY_ESC,              "Esc" },
  { 5,  KEY_LEFT_CTRL,        "Ctrl" },
  { 6,  KEY_LEFT_SHIFT,       "Shift" },
  { 7,  KEY_TAB,              "Tab" },
  { 8,  'w',                  "W" },
  { 9,  'a',                  "A" },
  { 10, 's',                  "S" },
  { 16, 'd',                  "D" },
  { 14, 'e',                  "E" },
  { 15, 'r',                  "R" },
  { 18, KEY_UP_ARROW,         "Up Arrow" },
  { 19, KEY_DOWN_ARROW,       "Down Arrow" },
  { 20, KEY_LEFT_ARROW,       "Left Arrow" },
  { 21, KEY_RIGHT_ARROW,      "Right Arrow" },
};

const int NUM_KEYS = sizeof(keys) / sizeof(keys[0]);
const unsigned long DEBOUNCE_MS = 5;

bool keyDown[NUM_KEYS] = {false};
bool lastReading[NUM_KEYS];
unsigned long lastChangeTime[NUM_KEYS] = {0};

void setup() {
  for (int i = 0; i < NUM_KEYS; i++) {
    pinMode(keys[i].pin, INPUT_PULLUP);
    lastReading[i] = HIGH;
  }
  Keyboard.begin();
}

void loop() {
  for (int i = 0; i < NUM_KEYS; i++) {
    bool reading = digitalRead(keys[i].pin);

    if (reading != lastReading[i]) {
      lastChangeTime[i] = millis();
      lastReading[i] = reading;
    }

    if (millis() - lastChangeTime[i] >= DEBOUNCE_MS) {
      bool shouldBeDown = (reading == LOW);
      if (shouldBeDown && !keyDown[i]) {
        Keyboard.press(keys[i].key);
        keyDown[i] = true;
      } else if (!shouldBeDown && keyDown[i]) {
        Keyboard.release(keys[i].key);
        keyDown[i] = false;
      }
    }
  }
}