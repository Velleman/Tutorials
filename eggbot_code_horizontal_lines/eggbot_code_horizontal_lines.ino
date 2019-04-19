#include <VarSpeedServo.h>

VarSpeedServo myservo;
VarSpeedServo myservo2;
VarSpeedServo myservo3;

const int servoPin = 3;
const int servoPin2 = 5;
const int servoPin3 = 6;

void setup() {
  myservo.attach(servoPin);
  myservo2.attach(servoPin2);
  myservo3.attach (servoPin3);
  myservo.write(150);
  myservo3.write(90);
  myservo2.write(55);
  delay(5000);

}

void loop() {

  myservo.write(150, 20, true);
  delay(0);
  myservo.write(38, 20, true);
  delay(0);

  myservo3.write(94);
  delay(300);
  myservo3.write(90);
  myservo.write(38, 20, true);
  delay(0);
  myservo.write(150, 20, true);
  delay(0);

  myservo3.write(94 );
  delay(300);
  myservo3.write(90);

}

