void momentary(int buttonIndex) {
// ternary operator is a concise way to write an if-else
  // if button read LOW write to transistor HIGH else LOW
  digitalWrite(transistorSwitches[buttonIndex], digitalRead(buttons[buttonIndex]) == LOW ? HIGH : LOW);
}