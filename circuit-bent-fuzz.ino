// Circuit Bent Fuzz / Distortion Controller
// meant to add buttons to a guitar pedal
// these buttons open and close circuit bends in a musical way
// first iteration will have one button to control 4 modes:
// 1. momentary
// 2. momentary tremolo
// 3. latching
// 4. latching tremelo
// two buttons and 3 transistor switches

// 2N7000 transistor switches
int transistorSwitches[] = { 5, 6 };

// 2 is for a switch to interupt the pedal's main led
int modeIndicator = 2;

// buttons
int modeButton = 10;
int buttons[] = { 3, 4 };

// default mode for switching
int mode = 1;

int millisReading[] = { 0, 0 };
int lastToggleTime[] = {
  0,
};
int tremDelay[] = { 60, 60 };
bool tremFlag[] = { false, false };

// extra variables for latching functions
const int NUM_BUTTONS = 2;
int buttonPins[NUM_BUTTONS] = { buttons[0], buttons[1] };
int reading[NUM_BUTTONS];
int lastButtonState[NUM_BUTTONS];
unsigned long lastDebounceTime[NUM_BUTTONS];
int buttonState[NUM_BUTTONS];
bool buttonPressed[NUM_BUTTONS];
int transistorState[NUM_BUTTONS];  // If each button controls a separate LED
unsigned long debounceDelay = 50;  // 50 milliseconds debounce time

// more for latching tremolo
unsigned long lastTremToggle[2] = { 0, 0 };         // per-button tremolo state variables
bool tremOn[2] = { false, false };                  // tremolo intervals
unsigned long latchingTremDelay[2] = { 100, 100 };  // ms for each button

void setup() {
  Serial.begin(9600);
  Serial.println("serial succes");

  for (int i = 0; i < 2; i++) {
    // setup buttons as inputs
    pinMode(buttons[i], INPUT_PULLUP);
    // setup transistor switches as outputs
    pinMode(transistorSwitches[i], OUTPUT);
  }
  // set mode button as input
  pinMode(modeButton, INPUT_PULLUP);
  // set up a switch to control the main led ~make it blink
  pinMode(modeIndicator, OUTPUT);

  digitalWrite(modeIndicator, HIGH);  // oopen the main led switch
}

void loop() {
  switch (mode) {
    case 1:
      // blinkMain(); if first loop
      momentary(0);
      momentary(1);
      break;

    case 2:
      momentaryTrem(0);
      momentaryTrem(1);
      break;

    case 3:
      // latching
      latching(0);  // first button
      latching(1);  // second button
      break;

    case 4:
      latchingTrem(0);
      latchingTrem(1);
      break;
  }

  // function to read the control button and change the definition of 'mode' variable
  setMode();
}

void setMode() {
  static unsigned long lastDebounceTime = 0;
  static int lastButtonState = HIGH;
  static int modeButtonState = HIGH;
  const unsigned long debounceDelay = 50;  // ms
  static int previousMode = -1;            // Track last mode for change detection

  int reading = digitalRead(modeButton);

  // only update debounce time if state changes
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  // if the button is LOW and it's been LOW for long enough, change mode
  if ((reading == LOW) && (millis() - lastDebounceTime > debounceDelay)) {
    // only toggle once per button press
    if (modeButtonState == HIGH) {
      if (mode >= 4) {
        mode = 0;
      }
      mode++;
      modeButtonState = LOW;

      // call blinkMain if mode changed
      if (mode != previousMode) {
        blinkMain();
        previousMode = mode;
      }
    }
  }

  // reset when button is released
  if (reading == HIGH) {
    // animation effect goes here
    modeButtonState = HIGH;
  }

  lastButtonState = reading;
}

void blinkMain() {
  digitalWrite(modeIndicator, !digitalRead(modeIndicator));  // toggle
}
