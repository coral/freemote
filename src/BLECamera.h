#pragma once

#include "bluefruit_common.h"
#include "BLECharacteristic.h"
#include "BLEClientCharacteristic.h"
#include "BLEClientService.h"

constexpr uint16_t SHUTTER_RELEASED = 0x0601;
constexpr uint16_t PRESS_TO_FOCUS = 0x0701;
constexpr uint16_t HOLD_FOCUS = 0x0801;
constexpr uint16_t TAKE_PICTURE = 0x0901;

class BLECamera : public BLEClientService
{
public:
    
    BLECamera(void);

    virtual bool begin(void);
    virtual bool discover(uint16_t conn_handle);


    bool enableNotify(void);
    bool disableNotify(void);

    bool trigger(void);

    //bool _ignorantTrigger(void);

protected:
    BLEClientCharacteristic _remoteCommand;
    BLEClientCharacteristic _remoteNotify;

    uint8_t _shutterStatus; //A0
    uint8_t _focusStatus; //3F
    uint8_t _recordingStatus; //D5

    uint32_t _last_message;


    void _handle_camera_notification(uint8_t* data, uint16_t len);

    friend void camera_notify_cb(BLEClientCharacteristic* chr, uint8_t* data, uint16_t len);
};