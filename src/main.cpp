#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "BLECamera.h"
#include "BLEHandler.h"

#define PIN PIN_NEOPIXEL
#define NUMPIXELS 1

Adafruit_NeoPixel pixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

BLECamera camera;

void setup()
{
    pixel.begin();
    pixel.setPixelColor(0, pixel.Color(0, 0, 2));
    pixel.show();

    Serial.begin(115200);
    while (!Serial)
        delay(10);
    
    pixel.setPixelColor(0, pixel.Color(0, 10, 0));
    pixel.show();

    if (!InitBLE(camera))
    {
        pixel.setPixelColor(0, pixel.Color(255, 0, 0));
        pixel.show();
    }
}

void loop()
{
    delay(5000);
    yield();
}