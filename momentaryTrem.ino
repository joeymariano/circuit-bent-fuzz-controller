void momentaryTrem(int buttonIndex) {
  if (digitalRead(buttons[buttonIndex]) == LOW) {
    // get the current time with millisReading
    millisReading[buttonIndex] = millis();

    // if (current time - lastToggleTime) >= tremoloInterval:
    if ((millisReading[buttonIndex] - lastToggleTime[buttonIndex]) >= tremDelay[buttonIndex]) {
      //  => flip the isOn state (if true, make false; if false, make true).
      if (tremFlag[buttonIndex] == false) {
        tremFlag[buttonIndex] = true;
      } else {
        tremFlag[buttonIndex] = false;
      }
      //  => update lastToggleTime to current time.
      lastToggleTime[buttonIndex] = millis();
    }

    // use tremFlag to decide switch on or off
    // need to use something to change switch
    if (tremFlag[buttonIndex]) {
      digitalWrite(transistorSwitches[buttonIndex], HIGH);
    } else {
      digitalWrite(transistorSwitches[buttonIndex], LOW);
    }
  } else {
    digitalWrite(transistorSwitches[buttonIndex], LOW);
  }
}