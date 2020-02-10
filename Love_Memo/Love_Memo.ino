#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <LiquidCrystal595.h>    // include the library
LiquidCrystal595 lcd(D6, D7, D8);   // data_pin, latch_pin, clock_pin

String LoveMessage;
bool newMessageReceived;

int screenWidth = 16;
int screenHeight = 2;
int n;
int i;

byte heart[8] = {
  B00000,
  B00000,
  B01010,
  B10101,
  B10001,
  B01010,
  B00100,
  B00000
};

int stringStart, stringStop = 0;
int scrollCursor = screenWidth;

AsyncWebServer server(80);

const int buttonPin = D4;     // the number of the pushbutton pin
const int ledPin =  D2;      // the number of the LED pin
const int ledPin2 =  D3;      // the number of the LED

int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

int buttonState = 0;         // variable for reading the pushbutton

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "YOUR WIFI NAME";
const char* password = "YOUR PASSWORD";

const char* PARAM_INPUT_1 = "input1";

// HTML web page to handle 1 input fields (input1,)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Love Memo</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    Type here your messsage:
    <br> 
    <input type="text" name="input1">
     <input type="submit" value="Send">
  </form>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);
  lcd.begin(screenWidth, screenHeight);

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  newMessageReceived = false;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());


  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {

    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      LoveMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      newMessageReceived = true;
    }
    else {
      LoveMessage = "No message sent";
      inputParam = "none";
    }

    request->send(200, "text/html", "HTTP GET request sent to your Lovebox ("
                  + inputParam + ") with text: " + LoveMessage +
                  "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (newMessageReceived) {
    fade();
    if (buttonState == LOW) {
      digitalWrite(ledPin, LOW);
      digitalWrite(ledPin2, LOW);
      n = (LoveMessage.length() + 17);
      for (i = 0; i < n; i++) {// Loop to do "something" n times
        scrollMessage();
      }
      newMessageReceived = false;
      delay(1000);
      lcd.clear();
    }
  }
}

void fade() {
  analogWrite(ledPin, brightness);
  analogWrite(ledPin2, brightness);

  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }
  delay(30);
}

void scrollMessage() {
  lcd.setCursor(scrollCursor, 1);
  lcd.print(LoveMessage.substring(stringStart, stringStop));
  lcd.createChar(0, heart);
  lcd.setCursor(0, 0);
  lcd.print(char(0));
  lcd.setCursor(2, 0);
  lcd.print(" YOUR LOVER ");
  lcd.setCursor(15, 0);
  lcd.print(char(0));
  delay(250);
  lcd.clear();
  if (stringStart == 0 && scrollCursor > 0) {
    scrollCursor--;
    stringStop++;
  } else if (stringStart == stringStop) {
    stringStart = stringStop = 0;
    scrollCursor = screenWidth;
  } else if (stringStop == LoveMessage.length() && scrollCursor == 0) {
    stringStart++;
  } else {
    stringStart++;
    stringStop++;
  }
}
