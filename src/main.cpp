#include <Arduino.h>

#include "BLECamera.h"
#include "BLEHandler.h"
#include "InputHandler.h"
#include "RemoteStatus.h"

namespace
{
BLECamera camera;

void resetBonds()
{
#if CFG_DEBUG
    Serial.println("Clearing bonds");
#endif
    BLEHandler::clearBondsAndRestart();
}

void checkForFactoryReset()
{
    pinMode(InputPins::ShutterButton, INPUT);
    pinMode(InputPins::FocusButton, INPUT);

    if (digitalRead(InputPins::ShutterButton) != HIGH ||
        digitalRead(InputPins::FocusButton) != HIGH)
    {
        return;
    }

    const uint32_t startedAt = millis();
    while (millis() - startedAt < 10000)
    {
        if (digitalRead(InputPins::ShutterButton) == LOW ||
            digitalRead(InputPins::FocusButton) == LOW)
        {
            return;
        }
        delay(100);
    }

#if CFG_DEBUG
    Serial.println("Factory reset requested");
#endif
    BLEHandler::clearBondsAndRestart();
}
} // namespace

void setup()
{
    pinMode(PIN_LED1, OUTPUT);

    RemoteStatus &status = RemoteStatus::access();
    if (!status.begin())
    {
        return;
    }
    status.set(Status::Boot);

#if CFG_DEBUG
    Serial.begin(115200);
    status.set(Status::WaitForSerial);
    while (!Serial)
    {
        delay(10);
    }
#endif

    checkForFactoryReset();

    if (!Input::begin(camera))
    {
        status.set(Status::Error);
        return;
    }
    Input::registerResetCallback(resetBonds);

    if (!BLEHandler::begin(camera))
    {
        status.set(Status::Error);
    }
}

void loop()
{
    yield();
    Input::process(millis());
}
