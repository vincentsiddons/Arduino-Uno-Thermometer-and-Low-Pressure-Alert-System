//Libraries for each non diode/digital component 
// Source: https://github.com/johnrickman/LiquidCrystal_I2C
#include <LiquidCrystal_I2C.h>
//Source: https://github.com/dhrubasaha08/DHT11
#include <DHT11.h>
//Source: https://github.com/adafruit/Adafruit_BMP280_Library/
#include <Adafruit_BMP280.h>

// Enabples I2C protocol communication
#include <Wire.h>

// Iterator
int i;

// Digital pins
int buzzerPin = 7;
int button1Pin = 8;
int button2Pin = 12;

// Creates LCD and Hum., Temp., and Pressure Sensor objects
LiquidCrystal_I2C lcd(0x27,16,2);
DHT11 sensor(2);
Adafruit_BMP280 bmp;

// Creates a degrees symbol char
byte degree[8] = {
    0b01110,
    0b01010,
    0b01110,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000

  };
void setup() {
  Wire.begin();
  // Initalizes LCD and turn on the backlight
  lcd.init();
  lcd.backlight();
  // Initalizes buzzer
  pinMode(buzzerPin, OUTPUT);
  //Initalizes buttons
  pinMode(button1Pin, OUTPUT);
  pinMode(button2Pin, OUTPUT);
  // Initalizes serial counter
  Serial.begin(9600);
  // Checks if BMP280 sensor is working
  // Source: https://github.com/adafruit/Adafruit_BMP280_Library/blob/master/examples/bmp280test/bmp280test.ino
  unsigned status;
  status = bmp.begin(0x76);
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }
  // Configures sampling settings for the BMP280
  // Source: https://github.com/adafruit/Adafruit_BMP280_Library/blob/master/examples/bmp280test/bmp280test.ino
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X2,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_OFF,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  

}



void loop() {
  // Reads temp., stores it, converts it to F
  float temp = bmp.readTemperature();
  temp = ((1.8)*temp + 32);
  // Reads humidity and stores it
  float hum = sensor.readHumidity();
  // Reads pressure and stores it
  float press = bmp.readPressure();
  // Converts to inhg. and adjusts for Altitude
  press = 0.00029530*press + 0.81;
  // Puts temp. and hum. on the LCD, along with the custom char
  if (temp != -1 and hum != - 1){
    // Prints pressure to Serial for debugging purposes
    Serial.print(press);
    Serial.print("  ");
    // Writes temp. and hum. to LCD when pressure is above 29.2 inhg
    lcd.setCursor(0,0);
    lcd.print("Temp.: ");
    lcd.print(temp, 2);
    lcd.createChar(0, degree);
    lcd.setCursor(12,0);
    lcd.write((byte)0);
    lcd.print("F");
    lcd.setCursor(0,1);
    lcd.print("Humidity: ");
    lcd.print(hum, 0);
    lcd.print(" %");
    // If button is pushed, give temperature in Celcius
    // You should have to push only for about 1.5 seconds MAX
    if (digitalRead(button1Pin) == HIGH){
      lcd.setCursor(7,0);
      lcd.print(bmp.readTemperature(), 2);
      lcd.setCursor(12,0);
      lcd.write((byte)0);
      lcd.print("C");
      //Keeps on display
      delay(2500);
    }
    // Checks if there is a Low Pressure System
    else if (press <= 29.5){
      // Delays for 2 hours to make sure it is a genuine Low
      delay(7200000);
      // Rings buzzer if button is not pressed
      if (digitalRead(button2Pin) == LOW){
        for(i = 0; i < 100; i++){
          digitalWrite(buzzerPin, HIGH);
          delay(2);
          digitalWrite(buzzerPin, LOW);
          delay(2);
        }
        // Prints alert to LCD of Low
        lcd.setCursor(0,0);
        lcd.print("Low Pres.");
        lcd.setCursor(0,1);
        lcd.print("Check radar");
      }
      // Activates when button is pressed
      // Prints message to LCD indicating the Low is known by you
      else if(digitalRead(button2Pin) == HIGH){
        lcd.setCursor(0,0);
        lcd.print("Low Known");
      }
    }
  } 
  // If the sensor is not working, return an error
  else{
    lcd.setCursor(0,0);
    lcd.print("ERROR");
  }
  // Update every 1.5 seconds
  delay(1500);

}
