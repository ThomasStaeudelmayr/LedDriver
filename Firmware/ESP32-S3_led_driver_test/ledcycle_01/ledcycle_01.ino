
void setup() {
  Serial.begin(9600);
}

void loop() {
  analogWrite(10, 127);
  analogWrite(13, 0);
  delay(1000);
  analogWrite(11, 127);
  analogWrite(10, 0);
  delay(1000);
  analogWrite(12, 127);
  analogWrite(11, 0);
  delay(1000);
  analogWrite(13, 127);
  analogWrite(12, 0);
  delay(1000);
  Serial.println("Hello");
}
