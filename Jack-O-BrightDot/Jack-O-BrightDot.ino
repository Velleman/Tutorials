#include <SimpleSDAudio.h>
#include <Adafruit_NeoPixel.h>

int pirPin = 6;
int pirState = 0;                   // PIR status

bool isPlaying = false;

#define LED_PIN    7
#define LED_COUNT 10

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  pinMode(pirPin, INPUT);
  Serial.begin(9600);
  SdPlay.setSDCSPin(10); // sd card cs pin
  SdPlay.init(SSDA_MODE_HALFRATE | SSDA_MODE_MONO | SSDA_MODE_AUTOWORKER);

  strip.setBrightness(250); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void loop() {
  pirState = digitalRead(pirPin);
  SdPlay.stop();
  analogWrite(9, LOW);

  if (pirState == HIGH) {
    analogWrite(9, HIGH);
    Serial.println(" Detected");
    SdPlay.setFile("test.wav");
    SdPlay.play();
    while (SdPlay.isPlaying()) {
      Serial.println("playing");
      strip.begin();
      strip.show();
      colorWipe(strip.Color(255,   255,   255), 0); // White
      delay(200);
      colorWipe(strip.Color(  0, 0,   0), 0); //
      delay(200);
    }
  } else {
    SdPlay.stop();
    analogWrite(9, LOW);
    colorWipe(strip.Color(  0, 0,   0), 50); //
  }

}
void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
