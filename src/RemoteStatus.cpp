#include "RemoteStatus.h"

RemoteStatus::RemoteStatus(void) : statusLed(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800)
{
    statusLed.begin();
    currentStatus = NONE;
    newStatus = NONE;
    update();
}

void RemoteStatus::set(Status s)
{
    newStatus = s;
}

void RemoteStatus::update(void)
{
    while(true)
    {
        delay(50);
        if (newStatus != DO_NOT_USE)
        {
            currentStatus = newStatus;
            newStatus = DO_NOT_USE;

            switch(currentStatus)
            {
                case BOOT:
                    statusLed.setPixelColor(0, statusLed.Color(0, 10, 0));
                    break;
            }

            statusLed.show();
        }

    }
}