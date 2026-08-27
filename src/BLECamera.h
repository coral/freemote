#pragma once

#include "BLEClientCharacteristic.h"
#include "BLEClientService.h"
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>

class RemoteStatus;

namespace SonyProtocol
{
inline constexpr uint16_t ShutterReleased = 0x0601;
inline constexpr uint16_t PressToFocus = 0x0701;
inline constexpr uint16_t HoldFocus = 0x0801;
inline constexpr uint16_t TakePicture = 0x0901;

inline constexpr std::array<uint8_t, 4> CameraManufacturer = {0x2D, 0x01, 0x03, 0x00};
} // namespace SonyProtocol

enum class Mode : uint8_t
{
    ManualFocus,
    AutoFocus,
};

enum class CameraState : uint8_t
{
    Disconnected,
    Connected,
    Discovered,
    Ready,
};

class BLECamera : public BLEClientService
{
public:
    BLECamera();

    bool begin() override;
    bool discover(uint16_t connHandle) override;

    bool enableNotify();
    bool disableNotify();

    bool pressTrigger();
    bool releaseTrigger();
    bool focus(bool pressed);
    bool release();

    bool isCamera(const uint8_t *data, size_t length) const;
    bool pairingStatus(const uint8_t *data, size_t length) const;

    void setMode(Mode mode);
    void onConnected();
    void onDisconnected();
    bool isReady() const;

protected:
    BLEClientCharacteristic remoteCommand_;
    BLEClientCharacteristic remoteNotify_;

    void handleCameraNotification(uint8_t *data, uint16_t length);

    friend void cameraNotifyCallback(BLEClientCharacteristic *characteristic, uint8_t *data, uint16_t length);

private:
    static constexpr uint32_t CommandTimeoutMs = 3000;

    bool writeCommand(uint16_t command);
    bool waitForStatus(const std::atomic<uint8_t> &status, uint8_t expected) const;

    RemoteStatus &status_;
    std::atomic<uint8_t> shutterStatus_{0};
    std::atomic<uint8_t> focusStatus_{0};
    std::atomic<uint8_t> recordingStatus_{0};
    std::atomic<CameraState> state_{CameraState::Disconnected};
    Mode mode_ = Mode::AutoFocus;
    std::atomic<bool> focusHeld_{false};
};
