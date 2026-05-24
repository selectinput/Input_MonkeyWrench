# Input_MonkeyWrench
Simple script(s) for using an ATmega32U4 board (Arduino Micro / Pro Micro / Leonardo) as a USB HID for a number of input projects.

## `input_monkeywrench.ino` — Keyboard HID

Each pin is `INPUT_PULLUP` — short pin to GND to trigger.

### Pin Map

| Pin | Key          |
|-----|--------------|
| 2   | Space        |
| 3   | Enter        |
| 4   | Esc          |
| 5   | Left Ctrl    |
| 6   | Left Shift   |
| 7   | Tab          |
| 8   | W            |
| 9   | A            |
| 10  | S            |
| 16  | D            |
| 14  | E            |
| 15  | R            |
| 18  | ↑ Arrow      |
| 19  | ↓ Arrow      |
| 20  | ← Arrow      |
| 21  | → Arrow      |

## `dual_throttle_quadrant/` — Game Controller HID

Adapts **two** old Saitek / Logitech "six-pin" Pro Flight throttle quadrants into a
single USB game controller (6 axes + 12 buttons). Each quadrant is passive: 3 levers
on potentiometers + 3 two-way momentary toggle switches (6 button contacts).

- **Board:** Pro Micro / Arduino Micro / Leonardo (ATmega32U4). In the Arduino IDE
  select **Arduino Leonardo** (or SparkFun Pro Micro 5V/16MHz).
- **Library:** "Joystick" by Matthew Heironimus (install via Library Manager).

### Wiring

Cut off the proprietary 6-pin connector and wire the internal terminals directly:

- **Potentiometers:** outer two lugs → board **5V** and **GND** (shared by both
  quadrants); center wiper → an analog pin.
- **Toggle switches:** each switch common → **GND**; its two contacts (up / down) →
  two digital pins (`INPUT_PULLUP`, pressed = LOW).

Pin numbers are an exact fit on a Pro Micro (all 18 broken-out pins used, so D0/D1
become buttons and hardware Serial is unavailable). Adjust the arrays at the top of
the sketch to match your wiring.

### Pin Map

| Signal                  | Pin(s)                    | HID            |
|-------------------------|---------------------------|----------------|
| Q1 levers 1 / 2 / 3     | A0 / A1 / A2              | axes X / Y / Z |
| Q2 levers 1 / 2 / 3     | A3 / A6 (D4) / A7 (D6)    | axes Rx/Ry/Rz  |
| Q1 buttons 1–6          | D2, D3, D5, D7, D8, D9    | buttons 0–5    |
| Q2 buttons 1–6          | D10, D14, D15, D16, D0, D1| buttons 6–11   |

On Windows, verify with **Run → `joy.cpl`** ("Set up USB game controllers"). If a
lever doesn't reach the rails, tune that axis's `rawMin`/`rawMax` in the sketch.
