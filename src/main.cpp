#include <Arduino.h>
#include "BLECamera.h"
#include "BLEHandler.h"
#include "RemoteStatus.h"
#include "InputHandler.h"

BLECamera camera;
BLEHandler handler;

void resetTest(void) {
    Serial.println("Clearing bonds");
    handler.clearBonds();
}

void setup()
{
    // Setup the red LED
    pinMode(PIN_LED1, OUTPUT);

    // Configure the Neopixel Status LED
    RemoteStatus *rs = RemoteStatus::access();
    rs->set(Status::BOOT);

    // Setup button handling
    Input::Init(&camera);

    Input::registerResetCallback(resetTest);

// Debug nation bro
#if CFG_DEBUG
    Serial.begin(115200);
    rs->set(Status::WAIT_FOR_SERIAL);
    while (!Serial)
        delay(10);
#endif

    // Initialze BLE
    if (!handler.InitBLE(&camera))
    {
        rs->set(Status::ERROR);
    }
}

void loop()
{
    // Netflix & Chill
    yield();

    // Check for button presses
    Input::process(millis());
}