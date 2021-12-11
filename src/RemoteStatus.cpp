#include "RemoteStatus.h"

RemoteStatus::RemoteStatus(void) : statusLed(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800)
{
    statusLed.begin();
    speed = 100;
    primaryColor = 0;
    secondaryColor = 0;
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
            statusLed.setPixelColor(0, primaryColor);
            statusLed.show();

            updateColor = false;
        }

        if (alternate)
        {
            if (!phase)
            {
                statusLed.setPixelColor(0, primaryColor);
                statusLed.show();
                phase = true;
            }
            else
            {
                statusLed.setPixelColor(0, secondaryColor);
                statusLed.show();
                phase = false;
            }
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
    case READY:
        prim.set(0, 128, 0);
        break;
    case FOCUS_ACQUIRED:
        prim.set(128, 128, 0);
        sec.set(10, 128, 0);
        alternate = true;
        speed = 100;
        break;
    // Deb
    case WAIT_FOR_SERIAL:
        prim.set(0, 255, 255);
        break;
    case DO_NOT_USE:
        prim.set(0, 0, 0);
        break;
    };

    primaryColor = statusLed.Color(prim.r, prim.g, prim.b);
    secondaryColor = statusLed.Color(sec.r, sec.g, sec.b);
}