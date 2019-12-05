//libraries
#include "U8glib.h"
#include <Wire.h>
#include "SparkFunBME280.h"
#include "SparkFunCCS811.h"
#define CCS811_ADDR 0x5B //Default I2C Address CCS811 sensor
#define BME280_ADDR 0x77 //Default I2C Adress BME280 sensor

//defenitions
BME280 sensorBME280;
CCS811 sensorCCS811(CCS811_ADDR);
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);	// Display which does not send AC

int CO2;
int TVOC;
float HUMIDITY_BME280;
float AMBIENTPRESSURE_BME280;
float AMBIENTPRESSURE_BME280_c; //converted to mBar (= Pa / 100);
float ALTITUDE_BME280;
float TEMP_BME280;
float seaLevelPressure = 101325; //pressure at sea level (Pa), needs to be set dayly for proper alitude value!
float TEMP_comp = -3.3;   //the EarthListener is warmed up from the TFT screen hence reporting a higher temperature. This compensation corrects this.
float HUMI_comp = 17;     //same for the humidity. Values are experimental and should be changed according to your findings.

#define LEDg 8    //greenLed
#define LEDr 7    //redLed
#define LEDb 9    //blueLed


void setup(void) 
{
  //start serial
  Serial.begin(9600);
  Serial.println("EarthListener mini");


  //set outputs for RGB led
  pinMode(LEDg, OUTPUT);     digitalWrite(LEDg, LOW);
  pinMode(LEDr, OUTPUT);     digitalWrite(LEDr, LOW);
  pinMode(LEDb, OUTPUT);     digitalWrite(LEDb, LOW);  
  controlLED('W');


  // flip screen, if required
  //u8g.setRot180();


  //start screen
  u8g_prepare();
  u8g.firstPage();  
  do {
    u8g.drawStr( 10, 10, "EarthListener mini"); 
    u8g.drawStr( 10, 20, "v1.0 by PSI");
  } while( u8g.nextPage() );


  //check status CCS811 sensor
  CCS811Core::status returnCode = sensorCCS811.begin();
  if (returnCode != CCS811Core::SENSOR_SUCCESS)
  {
    Serial.println("CCS811 sensor communication error.");
    while (1); //Hang if there was a problem.
  }


  //check status BME280 sensor & setup
  sensorBME280.setI2CAddress(BME280_ADDR); 
  Wire.begin();
  if (sensorBME280.beginI2C() == false) //Begin communication over I2C
  {
    Serial.println("BME280 sensor cummunication error.");
    while(1); //Freeze
  }
  sensorBME280.setFilter(1); //0 to 4 is valid. Filter coefficient. See 3.4.4
  sensorBME280.setStandbyTime(0); //0 to 7 valid. Time between readings. See table 27.
  sensorBME280.setTempOverSample(1); //0 to 16 are valid. 0 disables temp sensing. See table 24.
  sensorBME280.setPressureOverSample(1); //0 to 16 are valid. 0 disables pressure sensing. See table 23.
  sensorBME280.setHumidityOverSample(1); //0 to 16 are valid. 0 disables humidity sensing. See table 19.
  sensorBME280.setMode(MODE_NORMAL); //MODE_SLEEP, MODE_FORCED, MODE_NORMAL is valid. See 3.3
  sensorBME280.setReferencePressure(seaLevelPressure); //Adjust the sea level pressure used for altitude calculations. This should be a variable, not fixed!
  //If you do not set the correct sea level pressure for your location FOR THE CURRENT DAY it will not be able to calculate the altitude accurately!
  //Barometric pressure at sea level changes daily based on the weather!
  //read value from EEPROM if Temp is in °F or °C and lightning units are in km or mi


  //wait a short while
  delay(1000);  
}


void loop(void) 
{
  //read values from from BME280 sensor
  HUMIDITY_BME280 = sensorBME280.readFloatHumidity();
  AMBIENTPRESSURE_BME280 = sensorBME280.readFloatPressure();
  AMBIENTPRESSURE_BME280_c = AMBIENTPRESSURE_BME280 / 100; //convert Pa to mBar
  ALTITUDE_BME280 = sensorBME280.readFloatAltitudeMeters();
  TEMP_BME280 = sensorBME280.readTempC();
  //compensate temp & humi data
  TEMP_BME280 = TEMP_BME280 + TEMP_comp;
  if(HUMIDITY_BME280 <= (100 - HUMI_comp))  //make sure we don't have values > 100%
  {
    HUMIDITY_BME280 = HUMIDITY_BME280 + HUMI_comp;
  }


  //print data from BME280 to serial
  /*
    Serial.print("BME280   data:");
    Serial.print("                                 ");
    Serial.print("Temp: ");
    Serial.print( TEMP_BME280, 2);
    Serial.print("°C");
    Serial.print("     Humidity: ");
    Serial.print(HUMIDITY_BME280, 2);
    Serial.print("%");
    Serial.print("     Pressure: ");
    Serial.print( AMBIENTPRESSURE_BME280_c, 2);
    Serial.print("mBar");
    Serial.print("     Altitude: ");
    Serial.print( ALTITUDE_BME280, 2);
    Serial.println("m");
  */

  //read data from CCS811 sensor
  if (sensorCCS811.dataAvailable())
  {
    //Pass the temperature & humidity from BME280 sensor back into the CCS811 to compensate
    updateCCS811vars(TEMP_BME280,HUMIDITY_BME280);
    //read data from CCS811
    sensorCCS811.readAlgorithmResults(); //Calling this function updates the global tVOC and CO2 variables
    CO2 = sensorCCS811.getCO2();
    TVOC = sensorCCS811.getTVOC();
  }


  //print data from CCS811 to serial
  /*
  Serial.print("CCS811   data:");
  Serial.print(" eCO²: ");
  Serial.print(CO2);
  Serial.print("ppm");
  Serial.print("     TVOC: ");
  Serial.print(TVOC);
  Serial.println("ppb");
  */


  //show values on screen
  u8g.firstPage();  
  do {

    //print eCO2 value
    u8g.setPrintPos(10, 0);  u8g.print("eCO2:  "); u8g.print(CO2); u8g.print("ppm");
    
    //set led to eCO2 status & print quality to screen
    u8g.setPrintPos(25, 12 );
    if (CO2 < 600)
    {
      u8g.print("Excellent Air");
      controlLED('G');
    }
    else if ((CO2 >= 600) && (CO2 < 800))
    {
      u8g.print("  Good Air   ");
      controlLED('G');
    }
    else if ((CO2 >= 800) && (CO2 < 1000))
    {
      u8g.print("  Fair Air   ");
      controlLED('B');
    }
    else if ((CO2 >= 1000) && (CO2 < 1500))
    {
      u8g.print("Mediocre Air ");
      controlLED('Y');
    }
    else if (CO2 >= 1500)
    {
      u8g.print("   Bad Air   ");
      controlLED('R');
    }

  //print extra values (in scrolling text => to do)
  u8g.setPrintPos(10, 25); u8g.print("Temp:  "); u8g.print(TEMP_BME280); u8g.print((char)176); u8g.print("C");
  u8g.setPrintPos(10, 35); u8g.print("Humi:  "); u8g.print(HUMIDITY_BME280); u8g.print("%");
  u8g.setPrintPos(10, 45); u8g.print("Pres:  "); u8g.print(AMBIENTPRESSURE_BME280_c); u8g.print("mBar");
  u8g.setPrintPos(10, 55); u8g.print("TVOC:  "); u8g.print(TVOC); u8g.print("ppb");

  } while( u8g.nextPage() ); 


  //wait before running loop again
  delay(1000);
}


//update the CCS811 variables with the data from the BME280 sensor
void updateCCS811vars(float TEMP, float HUMIDITY)
{
  //Serial.println("Updating CCS811 sensor with enviroment values...");
  sensorCCS811.setEnvironmentalData( HUMIDITY, TEMP);
  //Serial.print("New humidity= "); Serial.print(HUMIDITY, 2); Serial.println("%");
  //Serial.print("New temp= "); Serial.print(TEMP, 2); Serial.println("°C");
}

//first settings for oled screen
void u8g_prepare(void) 
{
  u8g.setFont(u8g_font_6x10);
  u8g.setFontRefHeightExtendedText();
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
}

//change the LED color according to tatus
void controlLED(char COLOR)
{
      switch (COLOR) 
      {
        case 'R':     //red
          digitalWrite(LEDg, LOW);
          digitalWrite(LEDr, HIGH);
          digitalWrite(LEDb, LOW);
          break;
        case 'G':     //green
          digitalWrite(LEDg, HIGH);
          digitalWrite(LEDr, LOW);
          digitalWrite(LEDb, LOW);
          break;
        case 'Y':     //yellow
          digitalWrite(LEDg, HIGH);
          digitalWrite(LEDr, HIGH);
          digitalWrite(LEDb, LOW);
          break;
        case 'B':     //blue
          digitalWrite(LEDg, LOW);
          digitalWrite(LEDr, LOW);
          digitalWrite(LEDb, HIGH);
          break;
        case 'W':     //white
          digitalWrite(LEDg, HIGH);
          digitalWrite(LEDr, HIGH);
          digitalWrite(LEDb, HIGH);
          break;
        case 'O':     //off
          digitalWrite(LEDg, LOW);
          digitalWrite(LEDr, LOW);
          digitalWrite(LEDb, LOW);
          break;  
        default:
          digitalWrite(LEDg, LOW);
          digitalWrite(LEDr, LOW);
          digitalWrite(LEDb, LOW);
      }
}
