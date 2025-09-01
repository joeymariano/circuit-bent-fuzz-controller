// idea => hook main LED to arduino and use it to blink the mode
// use power from LED to power the arduino?
// blink once for mode 1 twice for mode 2 etc.

// transistor switches
int transistorSwitches[] = {5, 6};

// buttons
int modeButton = 10;
int buttons[] = {3, 4};

// default mode for switching
// 1. momentary
// 2. momentary tremolo
// 3. latching
// 4. latching tremelo
int mode = 1;

int millisReading[] = {0, 0};
int lastToggleTime[] = {0, };
int tremDelay[] = {60, 60};
bool tremFlag[] = {false, false};

// extra variables for latching functions
const int NUM_BUTTONS = 2;
int buttonPins[NUM_BUTTONS] = {buttons[0], buttons[1]};
int reading[NUM_BUTTONS];
int lastButtonState[NUM_BUTTONS];
unsigned long lastDebounceTime[NUM_BUTTONS];
int buttonState[NUM_BUTTONS];
bool buttonPressed[NUM_BUTTONS];
int transistorState[NUM_BUTTONS];         // If each button controls a separate LED
unsigned long debounceDelay = 50;  // 50 milliseconds debounce time

// more for latching tremolo
unsigned long lastTremToggle[2] = { 0, 0 };         // per-button tremolo state variables
bool tremOn[2] = { false, false };                  // tremolo intervals
unsigned long latchingTremDelay[2] = { 100, 100 };  // ms for each button

void setup() {
  Serial.begin(9600);
  Serial.println("serial succes");

  for (int i = 0; i < 2; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
    pinMode(transistorSwitches[i], OUTPUT);
  }

  pinMode(modeButton, INPUT_PULLUP);
}

void loop() {
  switch (mode) {
    case 1:
      // need to break this up like latching case
      momentary(0);
      momentary(1);
      break;

    case 2:
      // need to break this up like latching case
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
  const unsigned long debounceDelay = 50; // ms

  int reading = digitalRead(modeButton);

  // only update debounce time if state changes
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  // if the button is LOW and it's been LOW for long enough, change mode
  if ((reading == LOW) && (millis() - lastDebounceTime > debounceDelay)) {
    if (modeButtonState == HIGH) { // only toggle once per button press
      if (mode >= 4){
        mode = 0;
      }
      mode++;
      modeButtonState = LOW;
    }
  }

  // reset when button is released
  if (reading == HIGH) {
    modeButtonState = HIGH;
  }

  lastButtonState = reading;
}
