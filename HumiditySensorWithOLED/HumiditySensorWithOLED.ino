/*
CONNECTED MODULES:
- OLED Display 128x64
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
Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define TEXTSIZE 1
#define OFFSET_Y 45
int x = 0;
int pointsQueue[128];
int queueFull = 0;

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

//OTHER

#define HIGH_TEMPERATURE 38
#define LOW_TEMPERATURE 10
#define HIGH_TEMPERATURE_COMFORT 24.5
#define LOW_TEMPERATURE_COMFORT 22.5

float ledTemperature;

void LEDToHigh(int, float, int, int);
void LEDToLow(int, float, int, int);
void drawTempLine(int, const int*);

void setup() {
  if (!OLED.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    for (;;)
      ;
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

  if (temperature > HIGH_TEMPERATURE) {
    ledTemperature = HIGH_TEMPERATURE;
  } else if (temperature < LOW_TEMPERATURE) {
    ledTemperature = LOW_TEMPERATURE;
  } else {
    ledTemperature = temperature;
  }

  if (ledTemperature > LOW_TEMPERATURE_COMFORT && ledTemperature < HIGH_TEMPERATURE_COMFORT) {
    digitalWrite(GREEN_PIN, HIGH);
    analogWrite(RED_PIN, LOW);
    analogWrite(BLUE_PIN, LOW);
  } else {
    if (ledTemperature >= HIGH_TEMPERATURE_COMFORT) {
      LEDToHigh(RED_PIN, ledTemperature, HIGH_TEMPERATURE_COMFORT, HIGH_TEMPERATURE);
      LEDToLow(GREEN_PIN, ledTemperature, HIGH_TEMPERATURE_COMFORT, HIGH_TEMPERATURE);
      digitalWrite(BLUE_PIN, LOW);
    } else {
      LEDToLow(BLUE_PIN, ledTemperature, LOW_TEMPERATURE, LOW_TEMPERATURE_COMFORT);
      LEDToHigh(GREEN_PIN, ledTemperature, LOW_TEMPERATURE, LOW_TEMPERATURE_COMFORT);
      digitalWrite(RED_PIN, LOW);
    }
  }

  if (buttonValue == 0) {
    switchValue = !switchValue;
  }

  OLED.setTextSize(TEXTSIZE);
  OLED.setTextColor(SSD1306_WHITE);

  OLED.clearDisplay();
  OLED.setCursor(0, 0);

  if (switchValue == 0) {
    OLED.print("Temperature is ");
    OLED.print(temperature);
    OLED.println("C");
    
    pointsQueue[x] = (int)temperature;

    //DRAW GRAPHICS
    drawTempLine(x, pointsQueue);
    x++;
    if (x == SCREEN_WIDTH) {
      x = 0;
      if (queueFull == 0) {
        queueFull = 1;
      }
    }
    //END OF DRAW GRAPHICS
  } else {
    OLED.print("Humidity is ");
    OLED.print(humidity);
    OLED.println("%");
  }

  OLED.display();

  delay(PAUSE);
}

void LEDToHigh(int pinNumber, float currentTemp, int lowerTempLimit, int higherTempLimit) {
  analogWrite(pinNumber, (currentTemp - lowerTempLimit) * (255 / (higherTempLimit - lowerTempLimit)));
}

void LEDToLow(int pinNumber, float currentTemp, int lowerTempLimit, int higherTempLimit) {
  analogWrite(pinNumber, ((higherTempLimit - currentTemp) * (255 / (higherTempLimit - lowerTempLimit))));
}

void drawTempLine(int x, const int* pointsQueue) {
  for (int i = 0; i <= x; i++) {
    OLED.drawPixel(i, OFFSET_Y - pointsQueue[x - i], SSD1306_WHITE);
    if (queueFull == 1) {
      for (int j = x + 1; j < SCREEN_WIDTH; j++) {
        OLED.drawPixel(j, OFFSET_Y - pointsQueue[SCREEN_WIDTH - j + x], SSD1306_WHITE);
      }
    }
  }
}
