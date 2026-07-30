// Circuit Bent Fuzz / Distortion Controller
// Adds footswitches to a guitar fuzz/distortion that open & close circuit "bends"
// in a musical way, via 2N7000 low-side transistor switches on a Seeeduino XIAO SAMD21.
//
// 3 buttons:     MODE (cycles the bend behavior) + 2 bend buttons.
// 3 transistors: T1 interrupts the pedal's main LED; T2 & T3 are the two circuit bends.
//
// Mode indication is on the pedal's main LED via T1 (the "4LED rail"): pressing MODE
// blinks it the mode number (1..4) as brief dark pulses against its normally-on state.
//
// Bend modes cycled by MODE:
//   1 momentary       - bend engaged while the button is held
//   2 momentary-trem  - bend stutters while the button is held
//   3 latching        - press toggles the bend on/off
//   4 latching-trem   - press toggles a stuttering bend on/off

// ---------- pins (per manufactured PCB) ----------
const uint8_t PEDAL_LED = 1;   // T1/INTRPT gate (D1) - pedal main LED, rest HIGH (on), blinked for mode
const uint8_t MODE_BTN  = 9;   // D9

// ---------- tunables ----------
const unsigned long DEBOUNCE_MS   = 50;
const unsigned long BLINK_DARK_MS = 500;  // pedal LED dark time per mode-blink (the visible pulse)
const unsigned long BLINK_GAP_MS  = 500;  // gap (LED back on) between blinks
const unsigned long TREM_MS       = 60;   // momentary-tremolo interval
const unsigned long LATCH_TREM_MS = 100;  // latching-tremolo interval
const int NUM_MODES = 4;

#define LOG 1                             // 1 = print an event log over Serial @ 9600
#define T1_TEST 0                         // 1 = ignore normal op, just toggle T1 1s on/1s off
                                          //     to watch the pedal LED interrupt. Set 0 when done.
const char* MODE_NAMES[] = { "", "momentary", "momentary-trem", "latching", "latching-trem" };

// ---------- types (all structs before any function; Arduino prototypes need them) ----------
struct Button {              // debounced button, shared by all 3 buttons
  uint8_t pin;
  int lastRead;
  unsigned long lastChange;
  bool down;                 // debounced level: true = pressed
};

struct Bend {               // one circuit bend: button + transistor + latch/tremolo state
  Button button;
  uint8_t sw;                // transistor gate pin (OUTPUT)
  bool latched;              // latching modes
  bool tremPhase;            // tremolo square-wave phase
  unsigned long lastTrem;
};

// ---------- state ----------
Bend bends[2] = {
  { { 2, HIGH, 0, false }, 4, false, false, 0 },  // BTN-1 (D2) -> T2 (D4)  bend1
  { { 3, HIGH, 0, false }, 5, false, false, 0 },  // BTN-2 (D3) -> T3 (D5)  bend2
};

Button modeBtn = { MODE_BTN, HIGH, 0, false };
int mode = 1;

// mode indicator: blink the pedal LED (T1) N times = mode number.
// T1 rests HIGH (LED on); each blink is a dark pulse (T1 LOW). Non-blocking.
int blinkRemaining = 0;      // dark pulses still to emit
bool blinkDark = false;      // true while the LED is being held dark
unsigned long blinkStamp = 0;

// ---------- shared helpers ----------
// returns true once per debounced press (HIGH->LOW edge); also refreshes b.down
bool pressed(Button &b) {
  int r = digitalRead(b.pin);
  if (r != b.lastRead) { b.lastRead = r; b.lastChange = millis(); }
  bool edge = false;
  if (millis() - b.lastChange > DEBOUNCE_MS) {
    bool nowDown = (r == LOW);           // INPUT_PULLUP: LOW = pressed
    if (nowDown && !b.down) edge = true;
    b.down = nowDown;
  }
  return edge;
}

bool elapsed(unsigned long &since, unsigned long interval) {
  if (millis() - since >= interval) { since = millis(); return true; }
  return false;
}

void setSwitch(Bend &b, bool on) {
  digitalWrite(b.sw, on ? HIGH : LOW);
}

void tremolo(Bend &b, unsigned long interval) {
  if (elapsed(b.lastTrem, interval)) b.tremPhase = !b.tremPhase;
  setSwitch(b, b.tremPhase);
}

// ---------- mode-indicator blink (pedal LED / T1: HIGH = on, LOW = dark pulse) ----------
void startModeBlink(int count) {
  blinkRemaining = count;
  blinkDark = true;
  blinkStamp = millis();
  digitalWrite(PEDAL_LED, LOW);          // begin first dark pulse (LED off)
}

// advance the blink sequence without blocking; leaves the LED back ON when done
void serviceModeBlink() {
  if (!blinkDark && blinkRemaining == 0) return;   // idle
  if (blinkDark) {
    if (millis() - blinkStamp >= BLINK_DARK_MS) {
      digitalWrite(PEDAL_LED, HIGH);              // LED back on -> gap
      blinkDark = false;
      blinkStamp = millis();
      blinkRemaining--;
    }
  } else if (blinkRemaining > 0) {
    if (millis() - blinkStamp >= BLINK_GAP_MS) {
      digitalWrite(PEDAL_LED, LOW);               // next dark pulse
      blinkDark = true;
      blinkStamp = millis();
    }
  }
}

// ---------- bend + mode servicing ----------
void serviceBend(Bend &b, int idx) {
  bool justPressed = pressed(b.button);

  switch (mode) {
    case 1: setSwitch(b, b.button.down); break;                                 // momentary
    case 2: b.button.down ? tremolo(b, TREM_MS) : setSwitch(b, false); break;   // momentary-trem
    case 3: if (justPressed) b.latched = !b.latched;                            // latching
            setSwitch(b, b.latched); break;
    case 4: if (justPressed) b.latched = !b.latched;                            // latching-trem
            b.latched ? tremolo(b, LATCH_TREM_MS) : setSwitch(b, false); break;
  }

#if LOG
  if (justPressed) {
    Serial.print("BTN-"); Serial.print(idx + 1);
    Serial.print(" -> bend"); Serial.print(idx + 1);
    if (mode == 3 || mode == 4) Serial.println(b.latched ? " latched ON" : " latched OFF");
    else { Serial.print(' '); Serial.println(MODE_NAMES[mode]); }
  }
#endif
}

void serviceMode() {
  if (pressed(modeBtn)) {
    mode = (mode % NUM_MODES) + 1;   // cycle 1..4
    startModeBlink(mode);            // blink the pedal LED `mode` times
#if LOG
    Serial.print("MODE -> mode "); Serial.print(mode);
    Serial.print(" ("); Serial.print(MODE_NAMES[mode]); Serial.println(")");
#endif
  }
}

// T1 interrupt test: toggle the pedal LED gate (D1) slowly so the interrupt is obvious.
void t1BlinkTest() {
  static unsigned long last = 0;
  static bool on = true;
  if (millis() - last >= 1000) {
    last = millis();
    on = !on;
    digitalWrite(PEDAL_LED, on ? HIGH : LOW);
#if LOG
    Serial.println(on ? "T1 HIGH -> LED on" : "T1 LOW  -> LED should go DARK");
#endif
  }
}

// ---------- setup / loop ----------
void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 2; i++) {
    pinMode(bends[i].button.pin, INPUT_PULLUP);
    pinMode(bends[i].sw, OUTPUT);
    digitalWrite(bends[i].sw, LOW);
  }
  pinMode(modeBtn.pin, INPUT_PULLUP);

  pinMode(PEDAL_LED, OUTPUT);
  digitalWrite(PEDAL_LED, HIGH);   // ground leg closed -> pedal's LED works normally

#if LOG
  Serial.println("circuit-bent-fuzz ready");
  Serial.print("mode 1 ("); Serial.print(MODE_NAMES[1]); Serial.println(")");
#endif

  startModeBlink(mode);            // blink current mode once at boot
}

void loop() {
#if T1_TEST
  t1BlinkTest();   // watch the pedal LED interrupt; set T1_TEST 0 for normal operation
  return;
#endif

  serviceBend(bends[0], 0);
  serviceBend(bends[1], 1);
  serviceMode();
  serviceModeBlink();
}
