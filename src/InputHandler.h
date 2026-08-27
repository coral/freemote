#pragma once

#include "BLECamera.h"
#include <Arduino.h>

namespace InputPins
{
inline constexpr uint8_t ShutterButton = 11;
inline constexpr uint8_t FocusButton = 12;
inline constexpr uint8_t SelectSwitch = 13;
inline constexpr uint8_t ModeIndicator = 3;
} // namespace InputPins

using ButtonCallback = void (*)();

class Input
{
public:
    static bool begin(BLECamera &camera);
    static void process(uint32_t time);
    static void registerResetCallback(ButtonCallback callback);

private:
    static void readStartup();
    static void pressTrigger(uint8_t pin);
    static void releaseTrigger(uint8_t pin);
    static void pressFocus(uint8_t pin);
    static void releaseFocus(uint8_t pin);
    static void resetCheck(uint8_t pin, unsigned long duration);
    static void switchOn(uint8_t pin);
    static void switchOff(uint8_t pin);

    static inline BLECamera *camera_ = nullptr;
    static inline ButtonCallback resetCallback_ = nullptr;
    static inline bool canReset_ = true;
};
