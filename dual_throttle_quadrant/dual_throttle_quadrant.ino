// Two Saitek/Logitech Pro Flight throttle quadrants -> one USB game controller.
//
// Board: Pro Micro / Arduino Micro / Leonardo (ATmega32U4). Select "Arduino
// Leonardo" (or "SparkFun Pro Micro 5V/16MHz") in the IDE.
// Library: "Joystick" by Matthew Heironimus (MHeironimus ArduinoJoystickLibrary).
//
// Each quadrant is passive: 3 levers -> 3 potentiometers, 3 two-way momentary
// toggles -> 6 button contacts. Cut the proprietary 6-pin connector and wire the
// pot wipers to analog pins (outer pot lugs to 5V/GND) and the switch contacts to
// digital pins (switch commons to GND, read with INPUT_PULLUP).
//
// Pin use is an exact fit on a Pro Micro (all 18 pins), so D0/D1 are buttons and
// hardware Serial is unavailable. Edit the arrays below to match your wiring.

#include <Joystick.h>

// 6 axes (X, Y, Z, Rx, Ry, Rz), 12 buttons, no hat switch.
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_MULTI_AXIS,
                   12, 0,
                   true, true, true, true, true, true,
                   false, false, false, false, false);

struct Axis {
  uint8_t pin;
  int rawMin;   // analogRead value at the lever's low end
  int rawMax;   // analogRead value at the lever's high end
  const char* label;
};

// Order maps to X, Y, Z, Rx, Ry, Rz. A6/A7 are the analog channels on D4/D6.
Axis axes[] = {
  { A0, 0, 1023, "Q1 Lever 1" },
  { A1, 0, 1023, "Q1 Lever 2" },
  { A2, 0, 1023, "Q1 Lever 3" },
  { A3, 0, 1023, "Q2 Lever 1" },
  { A6, 0, 1023, "Q2 Lever 2" },
  { A7, 0, 1023, "Q2 Lever 3" },
};
const int NUM_AXES = sizeof(axes) / sizeof(axes[0]);

const uint8_t buttonPins[] = {
  2, 3, 5, 7, 8, 9,        // Q1 buttons 0-5
  10, 14, 15, 16, 0, 1,    // Q2 buttons 6-11
};
const int NUM_BUTTONS = sizeof(buttonPins) / sizeof(buttonPins[0]);

const unsigned long DEBOUNCE_MS = 5;
const int AXIS_OUT_MAX = 1023;       // HID axis output range is 0..AXIS_OUT_MAX
const int AXIS_CHANGE_THRESHOLD = 4; // ignore jitter smaller than this (output counts)

// Per-axis setters indexed to match the `axes` array order.
typedef void (Joystick_::*AxisSetter)(int32_t);
AxisSetter axisSetters[] = {
  &Joystick_::setXAxis,  &Joystick_::setYAxis,  &Joystick_::setZAxis,
  &Joystick_::setRxAxis, &Joystick_::setRyAxis, &Joystick_::setRzAxis,
};

int lastAxisValue[6];

bool buttonDown[12] = { false };
bool lastReading[12];
unsigned long lastChangeTime[12] = { 0 };

void setup() {
  for (int i = 0; i < NUM_AXES; i++) {
    pinMode(axes[i].pin, INPUT);
    lastAxisValue[i] = -1000;  // force first update
  }
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    lastReading[i] = HIGH;
  }

  Joystick.setXAxisRange(0, AXIS_OUT_MAX);
  Joystick.setYAxisRange(0, AXIS_OUT_MAX);
  Joystick.setZAxisRange(0, AXIS_OUT_MAX);
  Joystick.setRxAxisRange(0, AXIS_OUT_MAX);
  Joystick.setRyAxisRange(0, AXIS_OUT_MAX);
  Joystick.setRzAxisRange(0, AXIS_OUT_MAX);

  Joystick.begin();  // auto-send state is on by default
}

void loop() {
  for (int i = 0; i < NUM_AXES; i++) {
    int raw = analogRead(axes[i].pin);
    int value = map(raw, axes[i].rawMin, axes[i].rawMax, 0, AXIS_OUT_MAX);
    value = constrain(value, 0, AXIS_OUT_MAX);

    if (abs(value - lastAxisValue[i]) >= AXIS_CHANGE_THRESHOLD) {
      (Joystick.*axisSetters[i])(value);
      lastAxisValue[i] = value;
    }
  }

  for (int i = 0; i < NUM_BUTTONS; i++) {
    bool reading = digitalRead(buttonPins[i]);

    if (reading != lastReading[i]) {
      lastChangeTime[i] = millis();
      lastReading[i] = reading;
    }

    if (millis() - lastChangeTime[i] >= DEBOUNCE_MS) {
      bool shouldBeDown = (reading == LOW);
      if (shouldBeDown != buttonDown[i]) {
        Joystick.setButton(i, shouldBeDown);
        buttonDown[i] = shouldBeDown;
      }
    }
  }
}
