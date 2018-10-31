int GREEN_PIN=11;
int RED_PIN=10;
int BLUE_PIN=9;
int switchState = 0;
int sensorVal = analogRead(3);

void setup() {
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
}

void loop() 
{ 
    switchState = digitalRead(3);
    if (switchState == LOW) 
{
  // fade from green to red
  for(int i=0; i<255; i++) {
    analogWrite(RED_PIN, i);
    analogWrite(GREEN_PIN, 255-i);
    analogWrite(BLUE_PIN, 0);
    delay(30);
  }

  // fade from red to blue
  for(int i=0; i<255; i++) {
    analogWrite(RED_PIN, 255-i);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, i);
    delay(50);
  }

  // fade from blue to green
  for(int i=0; i<255; i++) {
    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, i);
    analogWrite(BLUE_PIN, 255-i);
    delay(50);
  }

  // fade from green to 0
  for(int i=0; i<255; i++) {
    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 0);
    delay(0);
}
}
}
