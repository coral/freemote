#define FASTLED_USE_GLOBAL_BRIGHTNESS 1
#include <FastLED.h>

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
    SHUTTER,

    WAIT_FOR_SERIAL,
    DO_NOT_USE,
};

class Color
{
public:
    void set(uint8_t nr, uint8_t ng, uint8_t nb)
    {
        r = nr;
        g = ng;
        b = nb;
    }
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

class RemoteStatus
{
    static inline RemoteStatus *instance = nullptr;

    RemoteStatus(void);

public:
    static RemoteStatus *access()
    {
        if (instance == nullptr)
            instance = new RemoteStatus;
        return instance;
    }

    void set(Status s);

private:
    void update();
    void resolveColor(Status s);

    CRGB leds[1];


    bool updateColor;
    Color primaryColor;
    Color secondaryColor;
    bool alternate;
    bool phase;
    uint32_t speed;

    TaskHandle_t statusLoopHandle;

    static void update_wrapper(void *arg)
    {
        instance->update();
    }
};
