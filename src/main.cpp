#include <Arduino.h>
#include "BLECamera.h"
#include "BLEHandler.h"
#include "RemoteStatus.h"
#include "InputDebounce.h"

#define BUTTON_PIN 7


BLECamera camera;
BLEHandler handler;

static InputDebounce triggerButton;

void triggerButton_pressedCallback(uint8_t pinIn) {
    Serial.println("Camera shutter pressed.");
    camera.trigger();
}


void setup()
{
    RemoteStatus *rs = rs->access();
    rs->set(Status::BOOT);

    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT);


#if CFG_DEBUG
    rs->set(Status::WAIT_FOR_SERIAL);
    while (!Serial)
        delay(10);
#endif

    // pixel.setPixelColor(0, pixel.Color(0, 10, 0));
    // pixel.show();

    if (!handler.InitBLE(&camera))
    {
        // pixel.setPixelColor(0, pixel.Color(255, 0, 0));
        // pixel.show();
    }

    triggerButton.registerCallbacks(triggerButton_pressedCallback, NULL, NULL, NULL);
	triggerButton.setup(BUTTON_PIN, 20, InputDebounce::PIM_INT_PULL_UP_RES, 0, InputDebounce::ST_NORMALLY_OPEN);
}

void loop()
{
    yield();
    triggerButton.process(millis());
}