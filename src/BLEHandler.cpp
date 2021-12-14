#include "BLEHandler.h"

RemoteStatus *rs = rs->access();

bool BLEHandler::InitBLE(BLECamera *newcam)
{

    _attempt_pairing = false;
    _camera_ref = newcam;

    Bluefruit.begin(0, 1);

    Bluefruit.setName("FREEMOTE");


    // Callbacks
    Bluefruit.Scanner.setRxCallback(_scan_callback);
    Bluefruit.Central.setConnectCallback(_connect_callback);
    Bluefruit.Central.setDisconnectCallback(_disconnect_callback);
    Bluefruit.Security.setSecuredCallback(_connection_secured_callback);

    VERIFY(_camera_ref->begin());

    Bluefruit.autoConnLed(false);
    #if CFG_DEBUG
    Bluefruit.autoConnLed(true);
    //Bluefruit.setConnLedInterval(250);
    #endif

    Bluefruit.Scanner.restartOnDisconnect(true);
    Bluefruit.Scanner.setInterval(160, 80);
    Bluefruit.Scanner.useActiveScan(false);
    Bluefruit.Scanner.start(0);

    Serial.println("Started scanning");

    rs->set(Status::CONNECTING);

    return true;
}

void BLEHandler::_scan_callback(ble_gap_evt_adv_report_t *report)
{
    std::array<uint8_t, 16> data;
    uint8_t bufferSize;

    if (report->data.len >= CAMERA_MANUFACTURER_LOOKUP.size())
    {

        bufferSize = Bluefruit.Scanner.parseReportByType(report, 0xff, data.data(), data.size());

        if (bufferSize >= CAMERA_MANUFACTURER_LOOKUP.size())
        {

            // Check if this is a Sony camera
            if (_camera_ref->isCamera(data))
            {
                // Check if camera wants to pair
                if (_camera_ref->pairingStatus(data))
                {
                    Serial.println("Camera wants to pair, lets do it");
                    _attempt_pairing = true;
                }

                Bluefruit.Central.connect(report);
            }
        }
    }
    Bluefruit.Scanner.resume();
}

void BLEHandler::_connect_callback(uint16_t conn_handle)
{
    BLEConnection *conn = Bluefruit.Connection(conn_handle);

    Serial.println("Connected to device.");
    if (_attempt_pairing == true)
    {
        conn->requestPairing();
    }
}

void BLEHandler::_disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
    (void)conn_handle;
    (void)reason;

    Serial.print("Disconnected, reason = 0x");
    Serial.println(reason, HEX);
    Bluefruit._setConnLed(true);
    rs->set(Status::CONNECTION_LOST);
}

void BLEHandler::_connection_secured_callback(uint16_t conn_handle)
{
    BLEConnection *conn = Bluefruit.Connection(conn_handle);

    if (!conn->secured())
    {
        Serial.println("Not secure");
        conn->requestPairing();
    }
    else
    {
        Serial.println("Secured");
        rs->set(Status::CONNECTED);

        if (_camera_ref->discover(conn_handle))
        {
            Serial.println("Found service");
        }
        else
        {
            Serial.println("Could not discover remote service");
            return;
        }

        delay(500);

        // Connecting camera
        if (_camera_ref->enableNotify())
        {
            Serial.println("Ready to control camera");
            rs->set(Status::READY);
            Bluefruit._setConnLed(false);
        }
        else
        {
            Serial.println("Could not enable notifications");
        }
    }
}

void BLEHandler::clearBonds(void) {
    Bluefruit.Central.clearBonds();
}