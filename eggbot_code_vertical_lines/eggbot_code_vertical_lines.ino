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
  myservo3.write(95);
  myservo2.write(58);
  delay(10000);
  myservo.write(150, 2, true);
  delay(0);
  myservo.write(40, 2, true);
  delay(0);
  myservo3.write(90);
}


void loop() {

}



