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
    virtual bool  discover(uint16_t conn_handle);

protected:
    BLEClientCharacteristic _remoteCommand;
    BLEClientCharacteristic _remoteNotify;
};