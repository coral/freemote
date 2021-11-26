#include "BLECamera.h"
#include "bluefruit.h"

BLECamera::BLECamera(void) : BLEClientService("8000FF00-FF00-FFFF-FFFF-FFFFFFFFFFFF"), _remoteCommand(0xFF01), _remoteNotify(0xFF02)
{
}

void camera_notify_cb(BLEClientCharacteristic *chr, uint8_t *data, uint16_t len)
{
    BLECamera &svc = (BLECamera &)chr->parentService();
    svc._handle_camera_notification(data, len);
}

bool BLECamera::begin(void)
{
    // Invoke base class begin()
    VERIFY(BLEClientService::begin());

    _remoteCommand.begin(this);

    _remoteNotify.setNotifyCallback(camera_notify_cb);
    _remoteNotify.begin(this);

    return true;
}

bool BLECamera::discover(uint16_t conn_handle)
{
    // Call Base class discover
    VERIFY(BLEClientService::discover(conn_handle));
    // VERIFY(_remoteNotify.discover());

    _conn_hdl = BLE_CONN_HANDLE_INVALID; // make as invalid until we found all chars

    // // Discover all characteristics
    Bluefruit.Discovery.discoverCharacteristic(conn_handle, _remoteCommand, _remoteNotify);
    // Serial.println(_remoteNotify.discovered());

    VERIFY(_remoteCommand.discovered() && _remoteNotify.discovered());

    _conn_hdl = conn_handle;

    return true;
}

void BLECamera::_handle_camera_notification(uint8_t *data, uint16_t len)
{
    //   varclr(&_last_kbd_report);
    //   memcpy(&_last_kbd_report, data, len);

    //   if ( _kbd_cb ) _kbd_cb(&_last_kbd_report);
    Serial.println("Camera data: ");
    Serial.print("LEN: ");
    Serial.print(len);
    Serial.print("  DATA: ");
    for (int n = 0; n < len; n++)
    {
        Serial.print(" ");
        Serial.print(data[n], HEX);
        Serial.print(" ");
    }
    // Serial.write(data, len);
    Serial.write("\0\n");
    Serial.flush();
}

bool BLECamera::enableNotify(void)
{
    return _remoteNotify.enableNotify();
}

bool BLECamera::disableNotify(void)
{
    return _remoteNotify.disableNotify();
}