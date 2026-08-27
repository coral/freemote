#pragma once

#include "BLECamera.h"
#include <bluefruit.h>

class BLEHandler
{
public:
    static bool begin(BLECamera &camera);
    static bool clearBondsAndRestart();

private:
    static bool initializeBluefruit();
    static void scanCallback(ble_gap_evt_adv_report_t *report);
    static void connectCallback(uint16_t connHandle);
    static void disconnectCallback(uint16_t connHandle, uint8_t reason);
    static void connectionSecuredCallback(uint16_t connHandle);

    static inline BLECamera *camera_ = nullptr;
    static inline bool attemptPairing_ = false;
    static inline bool initialized_ = false;
};
