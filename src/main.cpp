#include <Arduino.h>
#include <bluefruit.h>
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

void checkForFactoryReset(RemoteStatus *rs) {
    // Configure button pins for reading
    pinMode(SHUTTER_BUTTON_PIN, INPUT);
    pinMode(FOCUS_BUTTON_PIN, INPUT);

    // Check if both buttons are pressed at startup
    if (digitalRead(SHUTTER_BUTTON_PIN) == HIGH && digitalRead(FOCUS_BUTTON_PIN) == HIGH) {
        // Wait for 10 seconds while checking buttons are still held
        uint32_t startTime = millis();
        bool stillHeld = true;

        while ((millis() - startTime) < 10000) {
            if (digitalRead(SHUTTER_BUTTON_PIN) == LOW || digitalRead(FOCUS_BUTTON_PIN) == LOW) {
                stillHeld = false;
                break;
            }
            delay(100);
        }

        if (stillHeld) {
            // Both buttons held for 10 seconds - clear bonds
#if CFG_DEBUG
            Serial.println("Factory reset: Clearing all bonds");
#endif
            rs->set(Status::CLEARING_BONDS);

            // Flash yellow for 5 seconds
            delay(5000);

            // Bond storage lives on LittleFS, which is mounted by
            // Bluefruit.begin(). The normal BLE initialization happens later
            // in setup(), so initialize Bluefruit explicitly on this early
            // factory-reset path before touching the bond store.
            Bluefruit.begin(0, 1);

            // Clear BLE bonds
            Bluefruit.Central.clearBonds();

#if CFG_DEBUG
            Serial.println("Bonds cleared, restarting...");
#endif

            // Restart the device
            NVIC_SystemReset();
        }
    }
}

void setup()
{
    // Setup the red LED
    pinMode(PIN_LED1, OUTPUT);

    // Configure the Neopixel Status LED
    RemoteStatus *rs = RemoteStatus::access();
    rs->set(Status::BOOT);

// Debug nation bro
#if CFG_DEBUG
    Serial.begin(115200);
    rs->set(Status::WAIT_FOR_SERIAL);
    while (!Serial)
        delay(10);
#endif

    // Check for factory reset request (both buttons held)
    checkForFactoryReset(rs);

    // Setup button handling
    Input::Init(&camera);

    Input::registerResetCallback(resetTest);

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
