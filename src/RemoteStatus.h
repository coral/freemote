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

    Adafruit_NeoPixel statusLed;

    bool updateColor;
    uint32_t primaryColor;
    uint32_t secondaryColor;
    bool alternate;
    bool phase;
    uint32_t speed;

    TaskHandle_t statusLoopHandle;

    static void update_wrapper(void *arg)
    {
        instance->update();
    }
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