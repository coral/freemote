#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "BLECamera.h"
#include "BLEHandler.h"
#include "InputDebounce.h"

#define PIN PIN_NEOPIXEL
#define NUMPIXELS 1
#define BUTTON_PIN 7

Adafruit_NeoPixel pixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

BLECamera camera;

static InputDebounce triggerButton;

void triggerButton_pressedCallback(uint8_t pinIn) {
    Serial.println("pressed");
    camera.trigger();
}


void setup()
{
    pixel.begin();
    pixel.setPixelColor(0, pixel.Color(0, 0, 2));
    pixel.show();

    Serial.begin(115200);

    pinMode(BUTTON_PIN, INPUT);


#if CFG_DEBUG
    while (!Serial)
        delay(10);
#endif

    pixel.setPixelColor(0, pixel.Color(0, 10, 0));
    pixel.show();

    if (!InitBLE(camera))
    {
        pixel.setPixelColor(0, pixel.Color(255, 0, 0));
        pixel.show();
    }

    triggerButton.registerCallbacks(triggerButton_pressedCallback, NULL, NULL, NULL);
	triggerButton.setup(BUTTON_PIN, 20, InputDebounce::PIM_INT_PULL_UP_RES, 0, InputDebounce::ST_NORMALLY_OPEN);
}

void loop()
{
    yield();
    triggerButton.process(millis());
}