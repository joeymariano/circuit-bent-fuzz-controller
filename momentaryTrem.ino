void momentaryTrem(int buttonIndex) {
  if (digitalRead(buttons[buttonIndex]) == LOW) {
    // get the current time with millisReading
    millisReading[buttonIndex] = millis();

    // if (current time - lastToggleTime) >= tremoloInterval:
    if ((millisReading[buttonIndex] - lastToggleTime[buttonIndex]) >= tremDelay[buttonIndex]) {
      //  => toggle the tremFlag
      tremFlag[buttonIndex] = !tremFlag[buttonIndex];
      //  => update lastToggleTime to current time.
      lastToggleTime[buttonIndex] = millis();
    }

    // use tremFlag to decide switch on or off
    digitalWrite(transistorSwitches[buttonIndex], tremFlag[buttonIndex] ? HIGH : LOW);
  } else {
    digitalWrite(transistorSwitches[buttonIndex], LOW);
  }
}