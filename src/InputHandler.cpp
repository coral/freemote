#include "InputHandler.h"

bool Input::Init(BLECamera *newcam)
{
    _camera_ref = newcam;
    canReset = true;

    shutterButton.registerCallbacks(pressTrigger, NULL, NULL, resetCheck);
    focusButton.registerCallbacks(pressFocus, releaseFocus, NULL, NULL);
    selectSwitch.registerCallbacks(switch_on, switch_off, NULL, NULL);

    shutterButton.setup(SHUTTER_BUTTON_PIN, DEBOUNCE_DELAY, InputDebounce::PIM_EXT_PULL_DOWN_RES);
    focusButton.setup(FOCUS_BUTTON_PIN, DEBOUNCE_DELAY, InputDebounce::PIM_EXT_PULL_DOWN_RES);
    selectSwitch.setup(SELECT_SWITCH_PIN, DEBOUNCE_DELAY, InputDebounce::PIM_EXT_PULL_DOWN_RES);

    Input::readStartup();
}

void Input::readStartup(void)
{
    if (digitalRead(SELECT_SWITCH_PIN) == HIGH)
    {
        Input::switch_on(0);
    }
    else
    {
        Input::switch_off(0);
    }
}

void Input::process(unsigned long time)
{
    shutterButton.process(time);
    focusButton.process(time);
    selectSwitch.process(time);
}

void Input::pressTrigger(uint8_t pinIn)
{
    _camera_ref->trigger();
}

void Input::pressFocus(uint8_t pinIn)
{
    _camera_ref->focus(true);
}

void Input::releaseFocus(uint8_t pinIn)
{
    _camera_ref->focus(false);
}

void Input::resetCheck(uint8_t pinIn, unsigned long duration)
{
    if (duration > 10000 && millis() < 25000 && canReset)
    {
        // Clear bonds
        if (_resetCallback)
        {
            _resetCallback();
        }
    }
    canReset = false;
}

void Input::registerResetCallback(button_callback cb)
{
    _resetCallback = cb;
}

// Switch specific

void Input::switch_on(uint8_t pinIn)
{
    digitalWrite(3, HIGH);
    _camera_ref->setMode(MANUAL_FOCUS);
}

void Input::switch_off(uint8_t pinIn)
{
    digitalWrite(3, LOW);
    _camera_ref->setMode(AUTO_FOCUS);
}