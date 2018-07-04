// The library "Adafruit NeoPixel by Adafruit" needs to be installed.
#include <Adafruit_NeoPixel.h>
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// The LED strip is wired here
#define LED 3

// The sensor is wired to the 0 digital pin.
#define SENSOR 2

// Initialising the the NeoPixel strip library with the model we have
// installed.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(10, LED, NEO_GRBW + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  
  // Init Touch Sensor PIN
  pinMode(SENSOR, INPUT);
  
  // Init Strip
  strip.begin();
  
  // Setting the brightness of the LED strip to the highest level.
  strip.setBrightness(100);

  // Init sensor
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }

}

bool isOn = false;
uint32_t lastSwitch = millis();
uint32_t threshold = 1000;

void loop() {

  uint32_t currentTime = millis();
  uint32_t delta = currentTime - lastSwitch;
  
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  Serial.print(measure.RangeStatus);

  if (measure.RangeStatus != 4 && measure.RangeMilliMeter < 300 &&  measure.RangeMilliMeter > 0) {  // phase failures have incorrect data
    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);

    if (delta > threshold && isOn == true) {
      turnOff();
      isOn = false;
      lastSwitch = millis();
      delay(1500);
      return;
    }
    
    strip.setBrightness(100 - measure.RangeMilliMeter / 3);
    lightUp();
    lastSwitch = millis();
    isOn = true;
  }
 
  delay(100);

}

// Called to turn on the strip. 
// We only address the WHITE pixel.
void lightUp() {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0, 255));
    strip.show();
  }
}

// Called to turn off the strip.
void turnOff() {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0, 0));
    strip.show();
  }
}
