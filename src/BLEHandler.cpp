#include "BLEHandler.h"

#include "RemoteStatus.h"
#include <array>

bool BLEHandler::initializeBluefruit()
{
    if (initialized_)
    {
        return true;
    }

    initialized_ = Bluefruit.begin(0, 1);
    return initialized_;
}

bool BLEHandler::begin(BLECamera &camera)
{
    camera_ = &camera;
    attemptPairing_ = false;

    if (!initializeBluefruit())
    {
        return false;
    }

    Bluefruit.setName("FREEMOTE");
    Bluefruit.Scanner.setRxCallback(scanCallback);
    Bluefruit.Central.setConnectCallback(connectCallback);
    Bluefruit.Central.setDisconnectCallback(disconnectCallback);
    Bluefruit.Security.setSecuredCallback(connectionSecuredCallback);

    if (!camera_->begin())
    {
        return false;
    }

#if CFG_DEBUG
    Bluefruit.autoConnLed(true);
#else
    Bluefruit.autoConnLed(false);
#endif

    Bluefruit.Scanner.restartOnDisconnect(true);
    Bluefruit.Scanner.setInterval(160, 80);
    Bluefruit.Scanner.useActiveScan(false);

    if (!Bluefruit.Scanner.start(0))
    {
        return false;
    }

#if CFG_DEBUG
    Serial.println("Started scanning");
#endif
    RemoteStatus::access().set(Status::Connecting);
    return true;
}

void BLEHandler::scanCallback(ble_gap_evt_adv_report_t *report)
{
    if (camera_ == nullptr || report == nullptr)
    {
        Bluefruit.Scanner.resume();
        return;
    }

    std::array<uint8_t, 16> manufacturerData{};
    const uint8_t length = Bluefruit.Scanner.parseReportByType(
        report, BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA,
        manufacturerData.data(), manufacturerData.size());

    if (!camera_->isCamera(manufacturerData.data(), length))
    {
        Bluefruit.Scanner.resume();
        return;
    }

    attemptPairing_ = camera_->pairingStatus(manufacturerData.data(), length);
#if CFG_DEBUG
    if (attemptPairing_)
    {
        Serial.println("Camera is accepting a new remote");
    }
#endif

    if (!Bluefruit.Central.connect(report))
    {
        attemptPairing_ = false;
        Bluefruit.Scanner.resume();
    }
}

void BLEHandler::connectCallback(uint16_t connHandle)
{
    if (camera_ == nullptr)
    {
        return;
    }

    camera_->onConnected();
    BLEConnection *connection = Bluefruit.Connection(connHandle);

#if CFG_DEBUG
    Serial.println("Connected to camera");
#endif

    const bool shouldPair = attemptPairing_;
    attemptPairing_ = false;
    if (shouldPair && connection != nullptr)
    {
        connection->requestPairing();
    }
}

void BLEHandler::disconnectCallback(uint16_t connHandle, uint8_t reason)
{
    (void)connHandle;
    attemptPairing_ = false;
    if (camera_ != nullptr)
    {
        camera_->onDisconnected();
    }

#if CFG_DEBUG
    Serial.print("Disconnected, reason = 0x");
    Serial.println(reason, HEX);
#else
    (void)reason;
#endif
    RemoteStatus::access().set(Status::ConnectionLost);
}

void BLEHandler::connectionSecuredCallback(uint16_t connHandle)
{
    if (camera_ == nullptr)
    {
        return;
    }

    BLEConnection *connection = Bluefruit.Connection(connHandle);
    if (connection == nullptr)
    {
        RemoteStatus::access().set(Status::Error);
        return;
    }

    if (!connection->secured())
    {
#if CFG_DEBUG
        Serial.println("Connection is not secure; requesting pairing");
#endif
        if (!connection->requestPairing())
        {
            RemoteStatus::access().set(Status::Error);
        }
        return;
    }

    RemoteStatus::access().set(Status::Connected);
    if (!camera_->discover(connHandle))
    {
#if CFG_DEBUG
        Serial.println("Could not discover the camera remote service");
#endif
        RemoteStatus::access().set(Status::Error);
        return;
    }

    if (!camera_->enableNotify())
    {
#if CFG_DEBUG
        Serial.println("Could not enable camera notifications");
#endif
        RemoteStatus::access().set(Status::Error);
        return;
    }

#if CFG_DEBUG
    Serial.println("Camera is ready");
#endif
    RemoteStatus::access().set(Status::Ready);
}

bool BLEHandler::clearBondsAndRestart()
{
    RemoteStatus::access().set(Status::ClearingBonds);
    delay(5000);

    // Bluefruit.begin() mounts and initializes the LittleFS-backed bond store.
    // This path also runs before normal BLE setup on a factory-fresh board.
    if (!initializeBluefruit())
    {
        RemoteStatus::access().set(Status::Error);
        return false;
    }

    Bluefruit.Central.clearBonds();
    delay(100);
    NVIC_SystemReset();
    return true;
}
