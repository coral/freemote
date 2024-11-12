# Freemote - Sony Alpha BLE Remote implemented on the NRF52840

### [Watch the build on Youtube](https://www.youtube.com/watch?v=G_nyD2bTs7A)

![image](https://raw.githubusercontent.com/coral/freemote/master/_freemote_photo.jpg)

This is a BLE implementation for the NRF52840 that acts like a BLE (Bluetooth Low Energy) remote for the Sony Alpha series that support the BLE remotes similar to [JJC-RMT-P1BT](https://www.amazon.com/JJC-RMT-P1BT-Bluetooth-Wireless-Commander/dp/B08CR1QPKQ). I've tried this on a Sony A7 III but from my understanding it should work with ZV-1, RX100VII, ZV-E10, A7C, A6100, A6600, A1, A7RIV, A7SIII, A9II, 6400 and A7RIII as well.

Specifically this code runs on the [Adafruit NRF52840 Express](https://www.adafruit.com/product/4062) which costs about $25. The repo is a [PlatformIO](https://platformio.org/) project meaning if you open this it should handle dependencies and the toolchain to program your NRF52840. Currently the code will try to pair with whatever camera is open for pairing (described further down) and save that pairing and then automatically reconnect to that camera when avaliable. There is prob better ways of solving this but yeah, took the easy route here.

## Sony Alpha BLE Remote Protocol

When trying to discover the camera, you have to specifically scan for the "manufacturer data" as Sony uses this to indicate state etc for the camera. So for example an advertistment header might look like this: `0x2D, 0x01, 0x03, 0x00, 0x64, 0x00, 0x45, 0x31, 0x22, 0xEF, 0x00`. Decoding this you get:

| Code     | Meaning                                      |
|----------|----------------------------------------------|
| 2d 01    | Sony Corporation Company Identifier (0x012D) |
| 03 00    | Camera                                       |
| 64       | Protocol Version                             |
| 00       | ???                                          |
| 45 31    | Model Code                                   |
| 22 EF 00 | \<tag> CODE \</tag>                            |

The interesting part here is `0x22 0xEF 0x00` which indicates that the camera is open to pairing. If the camera is not in pairing mode you will get `0x22 0xAF 0x00` instead.

### Remote Control Service

We are specifically interested in consuming the `8000FF00-FF00-FFFF-FFFF-FFFFFFFFFFFF` service which contains two characteristics. 

#### Codes from remote (RemoteCommand 0xFF01)

This is what I initially thought and reflected in the variable names in the code:

| Code   | Description            |
|--------|------------------------|
| 0x0106 | Shutter released       |
| 0x0107 | Transitioning to focus |
| 0x0108 | Focus held             |
| 0x0109 | Shutter fully pressed  |

However, [Greg Leeds did a better job](https://gregleeds.com/reverse-engineering-sony-camera-bluetooth/) reversing this. With [additional testing script done](./protocol/ble-test.py) by [tao-j](https://github.com/tao-j), the best guesstimate of the command code is summarized in the following table. 

| Code     | Description     | 
| -------- | --------------- | 
| 0x0106   | Shutter Half Up   | 
| 0x0107   | Shutter Half Down | 
| 0x0108   | Shutter Fully Up  | 
| 0x0109   | Shutter Fully Down| 
| 0x010e   | Record Up   | 
| 0x010f   | Record Down  | 
| 0x0114   | AF-On Up    | 
| 0x0115   | AF-On Down  | 
| 0x0120   | C1 Up           | 
| 0x0121   | C1 Down         | 
| 0x0244[00~0f] | Zoom Tele Release | 
| 0x0245[10~8f] | Zoom Tele Press | 
| 0x0246[00~0f] | Zoom Wide Release | 
| 0x0247[10~8f] | Zoom Wide Press | 
| 0x026a[00~0f] | Focus In Release  | 
| 0x026b[00~7f] | Focus In Press | 
| 0x026c[00~0f] | Focus Out Release  | 
| 0x026d[00~7f] | Focus Out Press | 

In summary, those codes discovered so far have a length of either 2 or 3 bytes of the format:
```
+-----------------+-----------------+----------------------+
|     Length      |     Command     | Step size (optional) |
+-----------------+-----------------+----------------------+

```

The first byte `Length` is always present, it denotes how many bytes proceeding it: `0x01` for 1 byte, and `0x02` for two bytes. 
The second byte `Command` is the actual command, it either simulates a button or indicates a property change like zoom or focus.
The third byte `Step size` is optional, which is usually accompanied by a property change second byte to denote a step size. Known valid step sizes are listed in the bracket for each command, there could be more, only Sony engineering know that so far. 

Sadly, an exhaustive scan for `Command` codes from `0x00` to `0xff` revealed no extra hidden code.

For button codes, it's of best practice to issue them in pairs of `Down` and `Up`.
The shutter button commands must go through the sequence of "Half down -> Fully down -> Half up -> Fully up", otherwise the camera will be stuck in an inoperable state.


#### Answers from camera (RemoteCommand 0xFF02)

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


### Acknowledgments & Thanks

- [Greg Leeds](https://gregleeds.com/reverse-engineering-sony-camera-bluetooth/)
- [tao-j](https://github.com/tao-j) for further digging, reproing and discovering
