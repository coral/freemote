#include "BLEHandler.h"

BLECamera *_camera_ref;

bool InitBLE(BLECamera &newcam)
{
    _camera_ref = &newcam;

    Bluefruit.begin(0, 1);

    Bluefruit.setName("FREEMOTE");

    // Callbacks
    Bluefruit.Scanner.setRxCallback(_scan_callback);
    Bluefruit.Central.setConnectCallback(_connect_callback);
    Bluefruit.Central.setDisconnectCallback(_disconnect_callback);
    Bluefruit.Security.setSecuredCallback(_connection_secured_callback);

    VERIFY(_camera_ref->begin());

    Bluefruit.setConnLedInterval(250);

    Bluefruit.Scanner.restartOnDisconnect(true);
    Bluefruit.Scanner.setInterval(160, 80);
    Bluefruit.Scanner.useActiveScan(false);
    Bluefruit.Scanner.start(0);

    Serial.println("Started scanning");

    return true;
}

// is_camera returns true if this is a sony cam
bool is_camera(std::array<uint8_t, 16> data)
{
    return std::equal(lookup.begin(), lookup.end(), data.begin());
}

// pairing_status returns true if camera is open for pairing, false otherwise
bool pairing_status(std::array<uint8_t, 16> data)
{

    // We are certain this is a camera, lets check for pairing status
    auto it = std::find(data.begin(), data.end(), 0x22);
    if (it != data.end())
    {
        if (data.at((it - data.begin()) + 1) == 0xEF)
        {
            // Camera is ready to pair
            return true;
        }
        else
        {
            // Camera does not want to pair (yet)
            return false;
        }
    }
}

void _scan_callback(ble_gap_evt_adv_report_t *report)
{
    std::array<uint8_t, 16> data;
    uint8_t bufferSize;

    if (report->data.len >= lookup.size())
    {

        bufferSize = Bluefruit.Scanner.parseReportByType(report, 0xff, data.data(), data.size());

        if (bufferSize >= lookup.size())
        {

            // Check if this is a Sony camera
            if (is_camera(data))
            {
                //Check if camera wants to pair
                if (pairing_status(data))
                {
                    Serial.println("Found the camera, connecting.");
                    Bluefruit.Central.connect(report);
                }
                else
                {
                    Serial.println("NO PAIR 4Head");
                }
            }
        }
    }
    Bluefruit.Scanner.resume();
}

void _connect_callback(uint16_t conn_handle)
{
    BLEConnection *conn = Bluefruit.Connection(conn_handle);

    Serial.println("Connected to device.");

    conn->requestPairing();
}

void _disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
    (void)conn_handle;
    (void)reason;

    Serial.print("Disconnected, reason = 0x");
    Serial.println(reason, HEX);
}

void _connection_secured_callback(uint16_t conn_handle)
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
        }
        else
        {
            Serial.println("Could not enable notifications");
        }
    }
}