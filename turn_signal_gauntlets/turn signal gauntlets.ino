#include <Adafruit_NeoPixel.h>
#define PIN1 17
#define PIN2 16


int Touchpin = 32; // Connected to touch pin T8
int Touchpin2 = 12; // Connected to touch pin T5
int Maxvalue = 7; // When touchvalue is lower,.....
int Maxvalue2 = 13; // When touchvalue is lower,.....
int StartT8 = 10;
int StartT5 = 10;

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(12, PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(12, PIN2, NEO_GRB + NEO_KHZ800);
void setup()
{
  Serial.begin(9600);
  strip1.clear();
  strip1.show(); // Initialize all pixels to 'off'
  strip1.setBrightness(20);
  strip2.clear();
  strip2.show(); // Initialize all pixels to 'off'
  strip2.setBrightness(20);
}
void loop()
{
  Serial.println(touchRead(Touchpin));
  Serial.println(touchRead(Touchpin2));
  //
  for (int i = 0; i < 250; i++)
  {
    StartT8 += touchRead(Touchpin);
    StartT5 += touchRead(Touchpin2);
  }
  if (Maxvalue > touchRead(Touchpin)) {
    strip1.begin();
    colorWipe1(strip1.Color(255, 166, 2), 20); // Yellow
    colorWipe1(strip1.Color(0, 0, 0), 20); // Red
  }
 if (Maxvalue2 > touchRead(Touchpin2)) {
    strip2.begin();
    colorWipe2(strip2.Color(255, 166, 2), 20); // Yellow
    colorWipe2(strip2.Color(0, 0, 0), 20); // Red
  }
}
// Fill the dots one after the other with a color
void colorWipe1(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip1.numPixels(); i++) {
    strip1.setPixelColor(i, c);
    strip1.show();
    delay(wait);
  }
}
// Fill the dots one after the other with a color
void colorWipe2(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip2.numPixels(); i++) {
    strip2.setPixelColor(i, c);
    strip2.show();
    delay(wait);
  }
}
