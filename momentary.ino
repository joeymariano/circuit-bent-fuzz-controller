void momentary(int buttonIndex) {
  if (digitalRead(buttons[buttonIndex]) == LOW) {
    digitalWrite(transistorSwitches[buttonIndex], HIGH);
  } else {
    digitalWrite(transistorSwitches[buttonIndex], LOW);
  }
}