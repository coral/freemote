#include "BLECamera.h"
#include "bluefruit.h"

BLECamera::BLECamera(void) : BLEClientService("8000FF00-FF00-FFFF-FFFF-FFFFFFFFFFFF"), _remoteCommand(0xFF01), _remoteNotify(0xFF02)
{
    rs = rs->access();
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

    _conn_hdl = BLE_CONN_HANDLE_INVALID; // make as invalid until we found all chars

    // // Discover all characteristics
    Bluefruit.Discovery.discoverCharacteristic(conn_handle, _remoteCommand, _remoteNotify);

    VERIFY(_remoteCommand.discovered() && _remoteNotify.discovered());

    _conn_hdl = conn_handle;

    return true;
}

void BLECamera::_handle_camera_notification(uint8_t *data, uint16_t len)
{

#if CFG_DEBUG
    Serial.println("Camera data: ");
    Serial.print("LEN: ");
    Serial.print(len);
    Serial.print(" DATA: ");
    for (int n = 0; n < len; n++)
    {
        Serial.print(" ");
        Serial.print(data[n], HEX);
        Serial.print(" ");
    }
    Serial.write("\0\n");
    Serial.flush();
#endif

    if (len == 3)
    {
        if (data[0] == 0x02)
        {
            switch (data[1])
            {
            case 0x3F:
                _focusStatus = data[2];

                if (_focusStatus == 0x20)
                {
                    rs->set(Status::FOCUS_ACQUIRED);
                }
                else
                {
                    rs->set(Status::READY);
                }

                break;

            case 0xA0:
                _shutterStatus = data[2];

                if (_shutterStatus == 0x20)
                {
                    rs->set(Status::SHUTTER);
                }
                else
                {
                    rs->set(Status::READY);
                }

                break;

            case 0xD5:
                _recordingStatus = data[2];
                break;
            }

            _last_message = micros();
        }
    }
}

bool BLECamera::enableNotify(void)
{
    return _remoteNotify.enableNotify();
}

bool BLECamera::disableNotify(void)
{
    return _remoteNotify.disableNotify();
}

bool BLECamera::pressTrigger(void)
{
    // hack until I get this to work
    uint32_t timeout = millis() + 3000;

    if (!_focusHeld)
    {
        // Reset focus status
        _focusStatus = 0x00;

        // Focus
        _remoteCommand.write16_resp(PRESS_TO_FOCUS);

        if (mode == AUTO_FOCUS)
        {
            while (_focusStatus != 0x20)
            {
                yield();

                if (timeout < millis())
                {
                    break;
                }
            }
        }
    }

    // Release back to focus
    _remoteCommand.write16_resp(HOLD_FOCUS);

    // Reset focus status
    _shutterStatus = 0x00;

    // Shutter
    _remoteCommand.write16_resp(TAKE_PICTURE);

    if (mode == AUTO_FOCUS)
    {
        while (_shutterStatus != 0x20)
        {
            yield();

            if (timeout < millis())
            {
                break;
            }
        }
    }

    return true;
}

bool BLECamera::releaseTrigger(void)
{

    // Release back to focus
    _remoteCommand.write16_resp(HOLD_FOCUS);

    delay(10);

    // Let go?
    _remoteCommand.write16_resp(SHUTTER_RELEASED);
}


void BLECamera::focus(bool f)
{
    if (mode == AUTO_FOCUS)
    {
        _focusHeld = f;

        if (f)
        {
            // Focus
            _remoteCommand.write16_resp(PRESS_TO_FOCUS);
        }
        else
        {
            _remoteCommand.write16_resp(HOLD_FOCUS);
            delay(10);

            // Let go?
            _remoteCommand.write16_resp(SHUTTER_RELEASED);
        }
    }
}

void BLECamera::release(void)
{
    // Release back to focus
    _remoteCommand.write16_resp(HOLD_FOCUS);

    delay(10);

    // Let go?
    _remoteCommand.write16_resp(SHUTTER_RELEASED);
}

// is_camera returns true if this is a sony cam
bool BLECamera::isCamera(std::array<uint8_t, 16> data)
{
    return std::equal(CAMERA_MANUFACTURER_LOOKUP.begin(), CAMERA_MANUFACTURER_LOOKUP.end(), data.begin());
}

// pairing_status returns true if camera is open for pairing, false otherwise
bool BLECamera::pairingStatus(std::array<uint8_t, 16> data)
{

    // We are certain this is a camera, lets check for pairing status
    auto it = std::find(data.begin(), data.end(), 0x22);
    if (it != data.end() && (it + 1) != data.end())
    {
        if (*(it + 1) == 0xEF)
        {
            // Camera is ready to pair
            return true;
        }
    }

    // camera does not want to pair
    return false;
}

// This just sends the commands in order to test, doesn't work if the camera struggles to focus.
// bool BLECamera::_ignorantTrigger(void)
// {

//     // Focus
//     _remoteCommand.write16_resp(0x0701);

//     // Shutter
//     _remoteCommand.write16_resp(0x0901);

//     // Release back to focus
//     _remoteCommand.write16_resp(0x0801);

//     // Let go?
//     _remoteCommand.write16_resp(0x0601);

//     return true;
// }

void BLECamera::setMode(Mode m)
{
    _focusHeld = false;
    mode = m;
}