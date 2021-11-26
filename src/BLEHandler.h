#pragma once

#include "BLECamera.h"
#include <bluefruit.h>


constexpr uint8_t lookup[11] = {0x2D, 0x01, 0x03, 0x00, 0x64, 0x00,
 0x45, 0x31, 0x22, 0xEF, 0x00};

bool InitBLE(BLECamera& camera);

void _scan_callback(ble_gap_evt_adv_report_t *report);
void _connect_callback(uint16_t conn_handle);
void _disconnect_callback(uint16_t conn_handle, uint8_t reason);
void _connection_secured_callback(uint16_t conn_handle);