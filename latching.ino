void latching(int buttonIndex) {
  // take a reading from button
  reading[buttonIndex] = digitalRead(buttonPins[buttonIndex]);

  // if the current reading is different from the previous button state, 
  // reset debounce timer
  if (reading[buttonIndex] != lastButtonState[buttonIndex]) {
    // update the last debounce time for this button
    lastDebounceTime[buttonIndex] = millis();
  }

  // if current time is less than debounce time
  if ((millis() - lastDebounceTime[buttonIndex]) > debounceDelay) {
    // if button was just pressed (transitioned from HIGH to LOW), 
    // toggle LED state... do your thiiing.
    if (reading[buttonIndex] == LOW && buttonState[buttonIndex] == HIGH) {
      // toggle the transistor state
      transistorState[buttonIndex] = !transistorState[buttonIndex];
      digitalWrite(transistorSwitches[buttonIndex], transistorState[buttonIndex]);
      buttonPressed[buttonIndex] = true;
    } else if (reading[buttonIndex] == HIGH && buttonState[buttonIndex] == LOW) {
      buttonPressed[buttonIndex] = false;
    }
    buttonState[buttonIndex] = reading[buttonIndex];
  }

  // save current reading as the previous button state for next loop
  lastButtonState[buttonIndex] = reading[buttonIndex];
}