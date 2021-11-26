#include <Arduino.h>
#include <bluefruit.h>

// defintion
const uint8_t remoteServiceUUID[] =
    {0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0X80};

BLEUuid remoteServiceBLEUuid("8000FF00-FF00-FFFF-FFFF-FFFFFFFFFFFF");
BLEClientService remoteService(remoteServiceBLEUuid);
BLEClientCharacteristic remoteNotify(0xFF02);

uint8_t lookup[11] = {0x2D, 0x01, 0x03, 0x00, 0x64, 0x00, 0x45, 0x31, 0x22, 0xEF, 0x00};

void scan_callback(ble_gap_evt_adv_report_t *report)
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
                Serial.println("WE MATCHED: CONNECTING.");
                Bluefruit.Central.connect(report);
                Serial.flush();
            }
            else
            {
                // lol
            }
        }
    }
    Bluefruit.Scanner.resume();
}

/**
 * Callback invoked when an connection is established
 * @param conn_handle
 */
void connect_callback(uint16_t conn_handle)
{
    BLEConnection *conn = Bluefruit.Connection(conn_handle);

    Serial.println("Connected");

    conn->requestPairing();
}

void connection_secured_callback(uint16_t conn_handle)
{
    BLEConnection *conn = Bluefruit.Connection(conn_handle);

    if (!conn->secured())
    {
        Serial.println("not secure");
        conn->removeBondKey();
        conn->requestPairing();
    }
    else
    {
        Serial.println("Secured");

        if (remoteService.discover(conn_handle))
        {
            Serial.println("found service");
        }
        else
        {
            Serial.println("could not discover remote service");
            return;
        }

        if (remoteNotify.discover())
        {
            Serial.println("Discovered notification");
        }
        else
        {
            Serial.println("could not discover notify");
        }

        if (remoteNotify.enableNotify())
        {
            Serial.println("Ready to receive camera data");
        }
        else
        {
            Serial.println("Couldn't enable notify for camera data");
        }
    }
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
    (void)conn_handle;
    (void)reason;

    Serial.print("Disconnected, reason = 0x");
    Serial.println(reason, HEX);
}

void camera_callback(BLEClientCharacteristic *chr, uint8_t *data, uint16_t len)
{
    Serial.println("Camera data: ");
    for (int n = 0; n <= len; n++)
    {
        Serial.print(data[n], HEX);
    }
    //Serial.write(data, len);
    Serial.write("\0");
    Serial.flush();
}

void initBLE()
{
    Serial.begin(115200);
    while (!Serial)
        delay(10);
    Serial.println("Starting.");

    Bluefruit.begin(0, 1);

    Bluefruit.Central.setConnectCallback(connect_callback);
    Bluefruit.Central.setDisconnectCallback(disconnect_callback);
    Bluefruit.Security.setSecuredCallback(connection_secured_callback);

    remoteService.begin();

    remoteNotify.setNotifyCallback(camera_callback);
    remoteNotify.begin();

    Bluefruit.setConnLedInterval(250);

    Bluefruit.Scanner.setRxCallback(scan_callback);
    Bluefruit.Scanner.restartOnDisconnect(true);
    Bluefruit.Scanner.setInterval(160, 80); // in unit of 0.625 ms
    Bluefruit.Scanner.useActiveScan(false);
    Bluefruit.Scanner.start(0);
}

void setup()
{

    initBLE();
}

void loop()
{
    delay(100);
    yield();
}