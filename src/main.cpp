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


    if (!handler.InitBLE(&camera))
    {
        rs->set(Status::ERROR);
    }

    triggerButton.registerCallbacks(triggerButton_pressedCallback, NULL, NULL, NULL);
	triggerButton.setup(BUTTON_PIN, 20, InputDebounce::PIM_INT_PULL_UP_RES, 0, InputDebounce::ST_NORMALLY_OPEN);
}

void loop()
{
    yield();
    triggerButton.process(millis());
}