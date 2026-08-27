#pragma once

#define FASTLED_USE_GLOBAL_BRIGHTNESS 1
#include <Arduino.h>
#include <FastLED.h>
#include <queue.h>

enum class Status : uint8_t
{
    None,
    Boot,
    Error,
    Connecting,
    Connected,
    ConnectionLost,
    Ready,
    FocusAcquired,
    Shutter,
    ClearingBonds,
    WaitForSerial,
};

class RemoteStatus
{
public:
    static RemoteStatus &access();

    bool begin();
    void set(Status status);

private:
    struct Animation
    {
        CRGB primary = CRGB::Black;
        CRGB secondary = CRGB::Black;
        TickType_t interval = pdMS_TO_TICKS(50);
        bool alternate = false;
    };

    RemoteStatus() = default;

    static Animation resolve(Status status);
    static void updateWrapper(void *arg);
    void update();

    CRGB leds_[1] = {CRGB::Black};
    QueueHandle_t statusQueue_ = nullptr;
    TaskHandle_t statusTask_ = nullptr;
};
