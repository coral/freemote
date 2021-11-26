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

protected:
    BLEClientCharacteristic _remoteCommand;
    BLEClientCharacteristic _remoteNotify;

    void _handle_camera_notification(uint8_t* data, uint16_t len);

    friend void camera_notify_cb(BLEClientCharacteristic* chr, uint8_t* data, uint16_t len);
};