#include <Adafruit_NeoPixel.h>

#pragma once

enum Status
{   
    NONE,
    BOOT,
    ERROR,
    CONNECTING,
    CONNECTED,
    CONNECTION_LOST,

    READY,
    FOCUS_ACQUIRED,

    WAIT_FOR_SERIAL,
    DO_NOT_USE,
};

class RemoteStatus
{
    static inline RemoteStatus *instance;

    RemoteStatus(void);

public:
    static RemoteStatus *access()
    {
        if (!instance)
            instance = new RemoteStatus;
        return instance;
    }

    void set(Status s);

private:

    void update(void);

    Adafruit_NeoPixel statusLed;
    Status currentStatus;
    Status newStatus;
};
