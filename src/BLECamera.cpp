#include "BLECamera.h"

#include "RemoteStatus.h"

#include <Arduino.h>
#include <algorithm>
#include <bluefruit.h>

BLECamera::BLECamera()
    : BLEClientService("8000FF00-FF00-FFFF-FFFF-FFFFFFFFFFFF"),
      remoteCommand_(0xFF01),
      remoteNotify_(0xFF02),
      status_(RemoteStatus::access())
{
}

void cameraNotifyCallback(BLEClientCharacteristic *characteristic, uint8_t *data, uint16_t length)
{
    auto &service = static_cast<BLECamera &>(characteristic->parentService());
    service.handleCameraNotification(data, length);
}

bool BLECamera::begin()
{
    if (!BLEClientService::begin())
    {
        return false;
    }

    remoteCommand_.begin(this);
    remoteNotify_.setNotifyCallback(cameraNotifyCallback);
    remoteNotify_.begin(this);
    return true;
}

bool BLECamera::discover(uint16_t connHandle)
{
    if (!BLEClientService::discover(connHandle))
    {
        return false;
    }

    _conn_hdl = BLE_CONN_HANDLE_INVALID;
    const uint8_t discovered = Bluefruit.Discovery.discoverCharacteristic(
        connHandle, remoteCommand_, remoteNotify_);

    if (discovered != 2 || !remoteCommand_.discovered() || !remoteNotify_.discovered())
    {
        return false;
    }

    _conn_hdl = connHandle;
    state_.store(CameraState::Discovered);
    return true;
}

void BLECamera::handleCameraNotification(uint8_t *data, uint16_t length)
{
#if CFG_DEBUG
    Serial.print("Camera data (len ");
    Serial.print(length);
    Serial.print("): ");
    for (uint16_t index = 0; index < length; ++index)
    {
        Serial.print(data[index], HEX);
        Serial.print(' ');
    }
    Serial.println();
#endif

    if (data == nullptr || length != 3 || data[0] != 0x02)
    {
        return;
    }

    switch (data[1])
    {
    case 0x3F:
        focusStatus_.store(data[2]);
        status_.set(data[2] == 0x20 ? Status::FocusAcquired : Status::Ready);
        break;
    case 0xA0:
        shutterStatus_.store(data[2]);
        status_.set(data[2] == 0x20 ? Status::Shutter : Status::Ready);
        break;
    case 0xD5:
        recordingStatus_.store(data[2]);
        break;
    default:
        break;
    }
}

bool BLECamera::enableNotify()
{
    if (state_.load() != CameraState::Discovered || !remoteNotify_.enableNotify())
    {
        return false;
    }

    state_.store(CameraState::Ready);
    return true;
}

bool BLECamera::disableNotify()
{
    if (!isReady())
    {
        return false;
    }

    const bool disabled = remoteNotify_.disableNotify();
    if (disabled)
    {
        state_.store(CameraState::Discovered);
    }
    return disabled;
}

bool BLECamera::pressTrigger()
{
    if (!isReady())
    {
        return false;
    }

    if (!focusHeld_.load())
    {
        focusStatus_.store(0);
        if (!writeCommand(SonyProtocol::PressToFocus))
        {
            return false;
        }

        if (mode_ == Mode::AutoFocus && !waitForStatus(focusStatus_, 0x20))
        {
            release();
            return false;
        }
    }

    shutterStatus_.store(0);
    if (!writeCommand(SonyProtocol::TakePicture))
    {
        release();
        return false;
    }

    if (mode_ == Mode::AutoFocus && !waitForStatus(shutterStatus_, 0x20))
    {
        release();
        return false;
    }

    return true;
}

bool BLECamera::releaseTrigger()
{
    return release();
}

bool BLECamera::focus(bool pressed)
{
    if (mode_ != Mode::AutoFocus || !isReady())
    {
        return false;
    }

    if (pressed)
    {
        const bool focused = writeCommand(SonyProtocol::PressToFocus);
        focusHeld_.store(focused);
        return focused;
    }

    focusHeld_.store(false);
    return release();
}

bool BLECamera::release()
{
    if (!isReady())
    {
        focusHeld_.store(false);
        return false;
    }

    const bool held = writeCommand(SonyProtocol::HoldFocus);
    delay(10);
    const bool released = writeCommand(SonyProtocol::ShutterReleased);
    focusHeld_.store(false);
    return held && released;
}

bool BLECamera::writeCommand(uint16_t command)
{
    return isReady() && remoteCommand_.write16_resp(command) == sizeof(command);
}

bool BLECamera::waitForStatus(const std::atomic<uint8_t> &status, uint8_t expected) const
{
    const uint32_t startedAt = millis();
    while (status.load() != expected)
    {
        if (millis() - startedAt >= CommandTimeoutMs || !isReady())
        {
            return false;
        }
        yield();
    }
    return true;
}

bool BLECamera::isCamera(const uint8_t *data, size_t length) const
{
    return data != nullptr && length >= SonyProtocol::CameraManufacturer.size() &&
           std::equal(SonyProtocol::CameraManufacturer.begin(),
                      SonyProtocol::CameraManufacturer.end(), data);
}

bool BLECamera::pairingStatus(const uint8_t *data, size_t length) const
{
    if (data == nullptr || length < 2)
    {
        return false;
    }

    const uint8_t *tag = std::find(data, data + length, 0x22);
    if (tag == data + length || tag + 1 == data + length)
    {
        return false;
    }

    constexpr uint8_t PairingEnabled = 0x40;
    constexpr uint8_t RemoteEnabled = 0x02;
    return (tag[1] & (PairingEnabled | RemoteEnabled)) == (PairingEnabled | RemoteEnabled);
}

void BLECamera::setMode(Mode mode)
{
    if (focusHeld_.load() && mode != mode_)
    {
        release();
    }
    mode_ = mode;
}

void BLECamera::onConnected()
{
    focusStatus_.store(0);
    shutterStatus_.store(0);
    recordingStatus_.store(0);
    focusHeld_.store(false);
    state_.store(CameraState::Connected);
}

void BLECamera::onDisconnected()
{
    focusStatus_.store(0);
    shutterStatus_.store(0);
    recordingStatus_.store(0);
    focusHeld_.store(false);
    state_.store(CameraState::Disconnected);
}

bool BLECamera::isReady() const
{
    return state_.load() == CameraState::Ready;
}
