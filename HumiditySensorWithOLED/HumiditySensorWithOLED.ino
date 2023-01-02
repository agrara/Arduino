/*
CONNECTED MODULES:
- OLED Display 128x32
- Humidity/Temperature sensor
- Push button
- 3 color LED

REQUIRED ADDITIONAL LIBRARIES
- Adafruit GFX
- Adafruit SSD1306
- DHT sensor library by Adafruit
*/


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

//OLED

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define TEXTSIZE 1

//HUMIDITY SENSOR

#define HUMIDITY_SENSOR_TYPE DHT11
#define HUMIDITY_PIN 2
#define PAUSE 200
DHT HT(HUMIDITY_PIN, HUMIDITY_SENSOR_TYPE);
float humidity;
float temperature;

//BUTTON

#define BUTTON_PIN 4
int switchValue = 0;
int buttonValue;

//LED

#define BLUE_PIN 3
#define GREEN_PIN 5
#define RED_PIN 6
#define HIGH_TEMPERATURE 35
#define LOW_TEMPERATURE 10
#define HIGH_TEMPERATURE_COMFORT 25.5
#define LOW_TEMPERATURE_COMFORT 23.5

float ledTemperature;

void setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }

  HT.begin();

  pinMode(BUTTON_PIN, INPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
}

void loop() {
  humidity = HT.readHumidity();
  temperature = HT.readTemperature();
  buttonValue = digitalRead(BUTTON_PIN);

  if (temperature > 35) {
    ledTemperature = 35;
  } else if (temperature < 10) {
    ledTemperature = 10;
  } else {
    ledTemperature = temperature;
  }
  if(ledTemperature > 23.5 && ledTemperature < 25.5){
    digitalWrite(GREEN_PIN, 1);
    analogWrite(RED_PIN, 40);
    analogWrite(BLUE_PIN, 40);
  }
  else{
    if(ledTemperature >= 25.5){
      analogWrite(RED_PIN, ((ledTemperature - HIGH_TEMPERATURE_COMFORT) * (255 / (HIGH_TEMPERATURE - HIGH_TEMPERATURE_COMFORT))));
      analogWrite(GREEN_PIN, ((HIGH_TEMPERATURE - ledTemperature) * (255 / (HIGH_TEMPERATURE - HIGH_TEMPERATURE_COMFORT))));
      digitalWrite(BLUE_PIN, 0);
    }
      else {
        analogWrite(BLUE_PIN, ((LOW_TEMPERATURE_COMFORT - ledTemperature) * (255 / (LOW_TEMPERATURE_COMFORT - LOW_TEMPERATURE))));
        analogWrite(GREEN_PIN, ((ledTemperature - LOW_TEMPERATURE) * (255 / (LOW_TEMPERATURE_COMFORT - LOW_TEMPERATURE))));
        digitalWrite(RED_PIN, 0);        
      }
  }

  if (buttonValue == 0) {
    switchValue = !switchValue;
  }

  display.setTextSize(TEXTSIZE);
  display.setTextColor(SSD1306_WHITE);

  display.clearDisplay();
  display.setCursor(0, 0);

  if (switchValue == 0) {
    display.print("Temperature is ");
    display.print(temperature);
    display.println("C");
  } else {
    display.print("Humidity is ");
    display.print(humidity);
    display.println("%");
  }
  display.display();
  delay(PAUSE);
}
