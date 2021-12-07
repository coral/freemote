#pragma once

#include "BLECamera.h"
#include <bluefruit.h>
#include <array>
#include <algorithm>


class BLEHandler
{
    public:
        static inline BLECamera* _camera_ref = nullptr;
        static bool InitBLE(BLECamera *newcam);

        //Internal stuff
        static void _scan_callback(ble_gap_evt_adv_report_t *report);
        static void _connect_callback(uint16_t conn_handle);
        static void _disconnect_callback(uint16_t conn_handle, uint8_t reason);
        static void _connection_secured_callback(uint16_t conn_handle);

        static inline bool _attempt_pairing;

};
