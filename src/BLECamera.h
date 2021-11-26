#pragma once

#include "bluefruit_common.h"
#include "BLECharacteristic.h"
#include "BLEClientCharacteristic.h"
#include "BLEClientService.h"

class BLECamera : public BLEClientService
{
public:
    
    BLECamera(void);

    virtual bool begin(void);
    virtual bool discover(uint16_t conn_handle);


    bool enableNotify(void);
    bool disableNotify(void);

    bool ignorantTrigger(void);

protected:
    BLEClientCharacteristic _remoteCommand;
    BLEClientCharacteristic _remoteNotify;

    uint8_t _shutterStatus; //A0
    uint8_t _focusStatus; //3F
    uint8_t _recordingStatus; //D5

    void _handle_camera_notification(uint8_t* data, uint16_t len);

    friend void camera_notify_cb(BLEClientCharacteristic* chr, uint8_t* data, uint16_t len);
};