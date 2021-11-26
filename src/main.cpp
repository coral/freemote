#include <Arduino.h>
#include "BLECamera.h"
#include "BLEHandler.h"

BLECamera camera;

void setup()
{

    Serial.begin(115200);
    while (!Serial)
        delay(10);

    InitBLE(camera);
}

void loop()
{
    delay(5000);
    Serial.println("Heartbeat");
    yield();
}