# Circuit Bent Fuzz / Distortion Controller

Adds footswitches to a guitar fuzz/distortion pedal that open and close circuit
"bends" in a musical way. A Seeeduino XIAO (SAMD21) drives three **2N7000** low-side
transistor switches: one interrupts the pedal's main LED (used as a mode indicator),
the other two toggle/stutter two circuit-bend points on the pedal.

## Prototype platform

Prototyped against a **LANDTONE DIY "Build Your Own Fuzz Pedal" kit** — a through-hole,
true-bypass fuzz/distortion in a 1590B enclosure (Volume + Drive controls, 9 V).
See [`docs/landtone-kit/`](docs/landtone-kit/) for kit details and where the controller
taps in; product listing: [Amazon B06XQL49G8](https://www.amazon.com/dp/B06XQL49G8).

## Controls

- **MODE button** — cycles through the four bend modes (1 → 4 → 1) and blinks the
  pedal's main LED the mode number (1–4) so you can tell which mode you're in.
- **BTN-1 / BTN-2** — work the two bends according to the current mode.

### Modes

| # | Mode              | Bend button behavior                         |
|---|-------------------|----------------------------------------------|
| 1 | momentary         | bend engaged while the button is held        |
| 2 | momentary tremolo | bend stutters while the button is held       |
| 3 | latching          | press toggles the bend on/off                |
| 4 | latching tremolo  | press toggles a stuttering bend on/off       |

## Pin map (Seeeduino XIAO SAMD21, per the manufactured PCB)

| Signal            | Pin | Net          | Notes                                  |
|-------------------|-----|--------------|----------------------------------------|
| MODE button       | D9  | MODE         | `INPUT_PULLUP`, to GND                  |
| BTN-1             | D2  | BTN-1        | `INPUT_PULLUP`, to GND                  |
| BTN-2             | D3  | BTN-2        | `INPUT_PULLUP`, to GND                  |
| T1 gate           | D1  | INTRPT       | interrupts the pedal's main LED         |
| T2 gate           | D4  | BND-1        | circuit bend 1                          |
| T3 gate           | D5  | BND-2        | circuit bend 2                          |

Each transistor gate is fed through a 220 Ω resistor. The buttons pull to GND.

## Hardware wiring notes

The transistors are **low-side switches** — they go in the **ground leg** of what
they control, never in the supply leg (an N-channel FET can't switch the high side).

For the LED interrupt (T1), the current must flow **drain → source**:

- **INTRPT (drain)** → the LED's cathode / ground-side leg (the more-positive side)
- **INTRPT_OUT (source)** → **ground** (common with the XIAO's ground)
- splice T1 **in series with** the pedal's own LED-ground switch (e.g. the 3PDT lug),
  not in place of it, so the pedal still controls on/off and T1 only blinks it

If drain and source are reversed, the MOSFET's body diode conducts and the LED will
never fully turn off (it only dims by ~0.5 V) — swap the two outer legs to fix it.

Note: a plain 2N7000 driven from 3.3 V logic is only marginally on (threshold spec up
to 3 V). It works for the ground-referenced LED; for bends bridging arbitrary pedal
nodes a relay / CD4066 / optocoupler is more reliable.

## Serial log

Connect at **9600 baud** for an event log of what each press does, e.g.:

```
MODE -> mode 3 (latching)
BTN-1 -> bend1 latched ON
```

Set `#define LOG 0` to compile it out. `#define T1_TEST 1` swaps in a standalone
slow on/off test of the T1 LED-interrupt for bench debugging.

## Build

Arduino IDE or `arduino-cli`, board **Seeeduino XIAO (SAMD21)**:

```
arduino-cli compile --fqbn Seeeduino:samd:seeed_XIAO_m0 .
arduino-cli upload  --fqbn Seeeduino:samd:seeed_XIAO_m0 -p <port> .
```

If upload fails to find the board, double-tap RESET to enter the bootloader, then
reselect the port.

## Repo contents

- `circuit-bent-fuzz.ino` — the firmware (single file)
- `SCH_Schematic1_*.pdf`, `SCH_Schematic2_*.pdf` — controller + power schematics
- `circuit-bent-fuzz*.svg` — enclosure / faceplate layouts
- `2N7000-orientation-reference.svg` — TO-92 footprint orientation guide
- `docs/landtone-kit/` — notes on the LANDTONE fuzz kit used to prototype
