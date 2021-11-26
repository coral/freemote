#include <Arduino.h>
#include "BLECamera.h"
#include "BLEHandler.h"

BLECamera camera;

void setup()
{

    Serial.begin(115200);

    InitBLE(camera);
    
}

void loop()
{
    delay(1000);
    Serial.println("hello");
    yield();
}