# Freemote - Sony Alpha (a7) BLE Remote implemented on the NRF52840

This is a BLE implementation for the NRF52840 that acts like a BLE remote for the Sony Alpha series that support the BLE remotes similar to [JJC-RMT-P1BT](https://www.amazon.com/JJC-RMT-P1BT-Bluetooth-Wireless-Commander/dp/B08CR1QPKQ). It's not perfect but does work.


## Sony Alpha BLE Remote Protocol

When trying to discover the camera, you have to specifically scan for the "manufacturer data" as Sony uses this to indicate state etc for the camera. So for example an advertistment header might look like this: `0x2D, 0x01, 0x03, 0x00, 0x64, 0x00, 0x45, 0x31, 0x22, 0xEF, 0x00`. Decoding this you get:

| Code     | Meaning                                      |
|----------|----------------------------------------------|
| 2d 01    | Sony Corporation Company Identifier (0x012D) |
| 03 00    | Camera                                       |
| 64       | Protocol Version                             |
| 00       | ???                                          |
| 45 31    | Model Code                                   |
| 22 EF 00 | <Tag> CODE </tag>                            |

The interesting part here is `0x22 0xEF 0x00` which indicates that the camera is open to pairing. If the camera is not in pairing mode you will get `0x22 0xAF 0x00` instead.

### Remote Control Service

We are specifically interested in consuming the `8000FF00-FF00-FFFF-FFFF-FFFFFFFFFFFF` service which contains two characteristics. 

#### Codes from remote (RemoteCommand 0xFF02)

| Code   | Description            |
|--------|------------------------|
| 0x0106 | Shutter released       |
| 0x0107 | Transitioning to focus |
| 0x0108 | Focus held             |
| 0x0109 | Shutter fully pressed  |

#### Answers from camera (RemoteCommand 0xFF01)

| Code          | Meaning           |
|---------------|-------------------|
| **Focus**     |                   |
| 0x02 3F 00    | Lost              |
| 0x02 3F 20    | Acquired          |
|               |                   |
| **Shutter**   |                   |
| 0x02 A0 00    | Ready             |
| 0x02 A0 20    | Shutter active    |
|               |                   |
| **Recording** |                   |
| 0x02 D5 00    | Recording stopped |
| 0x02 D5 20    | Recording started |