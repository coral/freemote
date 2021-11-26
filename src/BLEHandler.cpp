#include "BLEHandler.h"

BLECamera *_camera_ref;

bool InitBLE(BLECamera &newcam)
{
    _camera_ref = &newcam;

    Bluefruit.begin(0,1);

    Bluefruit.setName("FREEMOTE");

    _camera_ref->begin();

    //Callbacks
    Bluefruit.Scanner.setRxCallback(_scan_callback);
    Bluefruit.Central.setConnectCallback(_connect_callback);
    Bluefruit.Central.setDisconnectCallback(_disconnect_callback);
    Bluefruit.Security.setSecuredCallback(_connection_secured_callback);

    Bluefruit.Scanner.restartOnDisconnect(true);
    Bluefruit.Scanner.setInterval(160, 80);
    Bluefruit.Scanner.useActiveScan(true);
    Bluefruit.Scanner.start(0);

    Serial.println("Started scanning");
    
    return true;
}


void _scan_callback(ble_gap_evt_adv_report_t *report)
{
    uint8_t data[16];
    uint8_t bufferSize;

    if (report->data.len >= sizeof(lookup))
    {

        bufferSize = Bluefruit.Scanner.parseReportByType(report, 0xff, data, sizeof(data));

        if (bufferSize >= sizeof(lookup))
        {

            if (memcmp(&lookup, data, sizeof(lookup)) == 0)
            {
                // we matched
                Serial.println("Found the camera, connecting.");
                Bluefruit.Central.connect(report);
                Serial.flush();
            }
            else
            {
                // Camera is not in pairing mode so prob cannot connect.
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
            Serial.println("could not discover remote service");
            return;
        }

    }
}