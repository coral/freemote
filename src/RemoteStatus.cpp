#include "RemoteStatus.h"

RemoteStatus::RemoteStatus(void)
{
    FastLED.addLeds<NEOPIXEL, PIN_NEOPIXEL>(leds, 1);
    speed = 100;
    set(NONE);

    xTaskCreate(&update_wrapper, "update", 256, NULL, TASK_PRIO_LOW, &statusLoopHandle);
}

void RemoteStatus::set(Status s)
{
    resolveColor(s);
    updateColor = true;
}

void RemoteStatus::update()
{
    while (true)
    {
        delay(speed);
        if (updateColor)
        {
            leds[0] = CRGB(primaryColor.r, primaryColor.g, primaryColor.b);
            FastLED.show();
            updateColor = false;
        }

        if (alternate)
        {
            if (!phase)
            {
                leds[0] = CRGB(primaryColor.r, primaryColor.g, primaryColor.b);
                phase = true;
            }
            else
            {
                leds[0] = CRGB(secondaryColor.r, secondaryColor.g, secondaryColor.b);
                phase = false;
            }
            FastLED.show();
        }
    }
}

void RemoteStatus::resolveColor(Status s)
{
    Color prim;
    Color sec;
    alternate = false;
    phase = false;
    speed = 50;

    switch (s)
    {
    //General
    case NONE:
        prim.set(0, 0, 0);
        break;
    case BOOT:
        prim.set(10, 10, 10);
        break;
    case ERROR:
        prim.set(255, 0, 0);
        sec.set(5, 0, 0);
        alternate = true;
        speed = 100;
        break;
    
    //Connection
    case CONNECTING:
        prim.set(0, 0, 255);
        sec.set(0, 0, 2);
        alternate = true;
        speed = 1000;
        break;
    case CONNECTED:
        prim.set(0, 0, 255);
        break;
    case CONNECTION_LOST:
        prim.set(0, 0, 255);
        sec.set(128, 0, 0);
        alternate = true;
        speed = 800;
        break;
    
    //Usage
    case READY:
        prim.set(0, 128, 0);
        break;
    case FOCUS_ACQUIRED:
        prim.set(128, 128, 0);
        sec.set(10, 128, 0);
        alternate = true;
        speed = 100;
        break;

    // Debug
    case WAIT_FOR_SERIAL:
        prim.set(0, 10, 10);
        break;
    case DO_NOT_USE:
        prim.set(0, 0, 0);
        break;
    };

    primaryColor = prim;
    secondaryColor = sec;
}