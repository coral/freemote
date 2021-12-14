#include "InputHandler.h"

bool Input::Init(BLECamera *newcam) {
    _camera_ref = newcam;

    shutterButton.registerCallbacks(pressed, released, pressedDurationCallback, releasedDurationCallback);
    focusButton.registerCallbacks(pressed, released, pressedDurationCallback, releasedDurationCallback);
    selectSwitch.registerCallbacks(switch_on, switch_off, NULL, NULL);

    shutterButton.setup(SHUTTER_BUTTON_PIN, DEBOUNCE_DELAY, InputDebounce::PIM_EXT_PULL_DOWN_RES);
    focusButton.setup(FOCUS_BUTTON_PIN, DEBOUNCE_DELAY, InputDebounce::PIM_EXT_PULL_DOWN_RES);
    selectSwitch.setup(SELECT_SWITCH_PIN, DEBOUNCE_DELAY, InputDebounce::PIM_EXT_PULL_DOWN_RES);
}

void Input::process(unsigned long time) {
   shutterButton.process(time); 
   focusButton.process(time); 
   selectSwitch.process(time); 
}

void Input::pressed(uint8_t pinIn)
{
  // handle pressed state
  Serial.print("HIGH (pin: ");
  Serial.print(pinIn);
  Serial.println(")");
}

void Input::released(uint8_t pinIn)
{
  // handle released state
  Serial.print("LOW (pin: ");
  Serial.print(pinIn);
  Serial.println(")");
}

void Input::pressedDurationCallback(uint8_t pinIn, unsigned long duration)
{
  // handle still pressed state
  Serial.print("HIGH (pin: ");
  Serial.print(pinIn);
  Serial.print(") still pressed, duration ");
  Serial.print(duration);
  Serial.println("ms");
}

void Input::releasedDurationCallback(uint8_t pinIn, unsigned long duration)
{
  // handle released state
  Serial.print("LOW (pin: ");
  Serial.print(pinIn);
  Serial.print("), duration ");
  Serial.print(duration);
  Serial.println("ms");
}


//Switch specific

void Input::switch_on(uint8_t pinIn)
{
    digitalWrite(3, HIGH);  
}

void Input::switch_off(uint8_t pinIn)
{
    digitalWrite(3, LOW);  
}