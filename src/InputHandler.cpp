#include "InputHandler.h"

#include "InputDebounce.h"

namespace
{
constexpr uint16_t DebounceDelayMs = 20;
InputDebounce shutterButton;
InputDebounce focusButton;
InputDebounce selectSwitch;
} // namespace

bool Input::begin(BLECamera &camera)
{
    camera_ = &camera;
    canReset_ = true;

    shutterButton.registerCallbacks(pressTrigger, releaseTrigger, nullptr, resetCheck);
    focusButton.registerCallbacks(pressFocus, releaseFocus, nullptr, nullptr);
    selectSwitch.registerCallbacks(switchOn, switchOff, nullptr, nullptr);

    shutterButton.setup(InputPins::ShutterButton, DebounceDelayMs, InputDebounce::PIM_EXT_PULL_DOWN_RES);
    focusButton.setup(InputPins::FocusButton, DebounceDelayMs, InputDebounce::PIM_EXT_PULL_DOWN_RES);
    selectSwitch.setup(InputPins::SelectSwitch, DebounceDelayMs, InputDebounce::PIM_EXT_PULL_DOWN_RES);
    pinMode(InputPins::ModeIndicator, OUTPUT);

    readStartup();
    return true;
}

void Input::readStartup()
{
    if (digitalRead(InputPins::SelectSwitch) == HIGH)
    {
        switchOn(InputPins::SelectSwitch);
    }
    else
    {
        switchOff(InputPins::SelectSwitch);
    }
}

void Input::process(uint32_t time)
{
    shutterButton.process(time);
    focusButton.process(time);
    selectSwitch.process(time);
}

void Input::pressTrigger(uint8_t pin)
{
    (void)pin;
    if (camera_ != nullptr)
    {
        camera_->pressTrigger();
    }
}

void Input::releaseTrigger(uint8_t pin)
{
    (void)pin;
    if (camera_ != nullptr)
    {
        camera_->releaseTrigger();
    }
}

void Input::pressFocus(uint8_t pin)
{
    (void)pin;
    if (camera_ != nullptr)
    {
        camera_->focus(true);
    }
}

void Input::releaseFocus(uint8_t pin)
{
    (void)pin;
    if (camera_ != nullptr)
    {
        camera_->focus(false);
    }
}

void Input::resetCheck(uint8_t pin, unsigned long duration)
{
    (void)pin;
    if (duration > 10000 && millis() < 25000 && canReset_ && resetCallback_ != nullptr)
    {
        resetCallback_();
    }
    canReset_ = false;
}

void Input::registerResetCallback(ButtonCallback callback)
{
    resetCallback_ = callback;
}

void Input::switchOn(uint8_t pin)
{
    (void)pin;
    digitalWrite(InputPins::ModeIndicator, HIGH);
    if (camera_ != nullptr)
    {
        camera_->setMode(Mode::ManualFocus);
    }
}

void Input::switchOff(uint8_t pin)
{
    (void)pin;
    digitalWrite(InputPins::ModeIndicator, LOW);
    if (camera_ != nullptr)
    {
        camera_->setMode(Mode::AutoFocus);
    }
}
