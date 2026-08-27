#include "RemoteStatus.h"

RemoteStatus &RemoteStatus::access()
{
    static RemoteStatus instance;
    return instance;
}

bool RemoteStatus::begin()
{
    if (statusTask_ != nullptr)
    {
        return true;
    }

    FastLED.addLeds<NEOPIXEL, PIN_NEOPIXEL>(leds_, 1);

    statusQueue_ = xQueueCreate(1, sizeof(Status));
    if (statusQueue_ == nullptr)
    {
        return false;
    }

    if (xTaskCreate(updateWrapper, "status", 512, this, TASK_PRIO_LOW, &statusTask_) != pdPASS)
    {
        vQueueDelete(statusQueue_);
        statusQueue_ = nullptr;
        return false;
    }

    set(Status::None);
    return true;
}

void RemoteStatus::set(Status status)
{
    if (statusQueue_ != nullptr)
    {
        xQueueOverwrite(statusQueue_, &status);
    }
}

void RemoteStatus::updateWrapper(void *arg)
{
    static_cast<RemoteStatus *>(arg)->update();
}

void RemoteStatus::update()
{
    Animation animation = resolve(Status::None);
    bool showPrimary = true;

    leds_[0] = animation.primary;
    FastLED.show();

    while (true)
    {
        Status nextStatus;
        if (xQueueReceive(statusQueue_, &nextStatus, animation.interval) == pdTRUE)
        {
            animation = resolve(nextStatus);
            showPrimary = true;
            leds_[0] = animation.primary;
            FastLED.show();
        }
        else if (animation.alternate)
        {
            showPrimary = !showPrimary;
            leds_[0] = showPrimary ? animation.primary : animation.secondary;
            FastLED.show();
        }
    }
}

RemoteStatus::Animation RemoteStatus::resolve(Status status)
{
    Animation animation;

    switch (status)
    {
    case Status::None:
        break;
    case Status::Boot:
        animation.primary = CRGB(10, 10, 10);
        break;
    case Status::Error:
        animation.primary = CRGB(255, 0, 0);
        animation.secondary = CRGB(5, 0, 0);
        animation.interval = pdMS_TO_TICKS(100);
        animation.alternate = true;
        break;
    case Status::Connecting:
        animation.primary = CRGB(0, 0, 255);
        animation.secondary = CRGB(0, 0, 2);
        animation.interval = pdMS_TO_TICKS(1000);
        animation.alternate = true;
        break;
    case Status::Connected:
        animation.primary = CRGB(0, 0, 255);
        break;
    case Status::ConnectionLost:
        animation.primary = CRGB(0, 0, 255);
        animation.secondary = CRGB(128, 0, 0);
        animation.interval = pdMS_TO_TICKS(800);
        animation.alternate = true;
        break;
    case Status::Ready:
        animation.primary = CRGB(0, 128, 0);
        break;
    case Status::FocusAcquired:
        animation.primary = CRGB(128, 128, 0);
        animation.secondary = CRGB(10, 128, 0);
        animation.interval = pdMS_TO_TICKS(100);
        animation.alternate = true;
        break;
    case Status::Shutter:
        animation.primary = CRGB(255, 255, 255);
        break;
    case Status::ClearingBonds:
        animation.primary = CRGB(255, 255, 0);
        break;
    case Status::WaitForSerial:
        animation.primary = CRGB(0, 10, 10);
        break;
    }

    return animation;
}
