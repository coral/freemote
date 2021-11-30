#pragma once

#include "BLECamera.h"
#include <bluefruit.h>
#include <array>
#include <algorithm>


//0x2D01 = Sony Camera Corporation Identifer
//0x3000 = This is a camera
//0x64 = Protocol version ?
//0x00 = ??
constexpr std::array<uint8_t, 6> lookup = {0x2D, 0x01, 0x03, 0x00, 0x64, 0x00};

//ASCII Model Code E-mount cameras 'E1', A-mount 'A1'.
constexpr std::array<uint8_t, 2> model_code  = {0x45, 0x31};

//Indicates if camera is open to pair
//0x22 indicate tag, 0xEF pairing (with bluetooth remote), 0x00 end
constexpr std::array<uint8_t, 3> pairing = {0x22, 0xEF, 0x00};

//Call this once on startup
bool InitBLE(BLECamera& camera);

//Camera matching
bool is_camera(std::array<uint8_t, 16> data);
bool pairing_status(std::array<uint8_t, 16> data);

//Internal stuff
void _scan_callback(ble_gap_evt_adv_report_t *report);
void _connect_callback(uint16_t conn_handle);
void _disconnect_callback(uint16_t conn_handle, uint8_t reason);
void _connection_secured_callback(uint16_t conn_handle);
