void latchingTrem(int buttonIndex) {
  // latching logic (same as latching function)
  reading[buttonIndex] = digitalRead(buttonPins[buttonIndex]);

  if (reading[buttonIndex] != lastButtonState[buttonIndex]) {
    lastDebounceTime[buttonIndex] = millis();
  }

  if ((millis() - lastDebounceTime[buttonIndex]) > debounceDelay) {
    if (reading[buttonIndex] == LOW && buttonState[buttonIndex] == HIGH) {
      transistorState[buttonIndex] = !transistorState[buttonIndex]; // latch ON/OFF
      buttonPressed[buttonIndex] = true;
    } else if (reading[buttonIndex] == HIGH && buttonState[buttonIndex] == LOW) {
      buttonPressed[buttonIndex] = false;
    }
    buttonState[buttonIndex] = reading[buttonIndex];
  }
  
  lastButtonState[buttonIndex] = reading[buttonIndex];

  // tremolo logic **only if latched ON**
  if (transistorState[buttonIndex]) {
    unsigned long now = millis();
    if (now - lastTremToggle[buttonIndex] >= latchingTremDelay[buttonIndex]) {
      tremOn[buttonIndex] = !tremOn[buttonIndex];   // toggle the trem effect
      lastTremToggle[buttonIndex] = now;
    }
    // set output pin based on tremOn
    digitalWrite(transistorSwitches[buttonIndex], tremOn[buttonIndex]);
  } else {
    // if latched OFF, just turn off output
    digitalWrite(transistorSwitches[buttonIndex], tremOn[buttonIndex]);
    tremOn[buttonIndex] = false; // reset trem state (optional)
  }
}