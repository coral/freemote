import asyncio

import os
# os.environ["BLEAK_DBUS_AUTH_UID"] = "1000"

from bleak import BleakScanner, BleakClient

# commands simulate a button press
shu = b'\x01\x06'   # Shutter Half Up 
shd = b'\x01\x07'   # Shutter Half Down
sfu = b'\x01\x08'   # Shutter Fully Up
sfd = b'\x01\x09'   # Shutter Fully Down
ret = b'\x01\x0e'   # Record Up
rec = b'\x01\x0f'   # Record Down
afu = b'\x01\x14'   # AF-On Up
afd = b'\x01\x15'   # AF-On Down
c1u = b'\x01\x20'   # Custom Up 1
c1d = b'\x01\x21'   # Custom Down 1
# commands with step size
ozt = b'\x02\x44\x00'   # optical zoom tele?
dzt = b'\x02\x45\x10'   # digital zoom tele?
ozw = b'\x02\x46\x00'   # optical zoom wide?
dzw = b'\x02\x47\x10'   # digital zoom wide?
zib = b'\x02\x6a\x00'   # Zoom In??
fib = b'\x02\x6b\x00'   # Focus In
zob = b'\x02\x6c\x00'   # Zoom Out??
fob = b'\x02\x6d\x00'   # Focus Out

async def main(inc):
    # # discover sony cameras
    # sony_cams = []
    # scanner = BleakScanner()
    # devices = await scanner.discover(timeout=1)
    # for device in devices:
    #     if device.name.startswith("ILCE"):
    #         sony_cams.append(device.address)
    #         print(device.name, device.address)
    
    # directly connect to known paired cameras
    # https://support.d-imaging.sony.co.jp/www/cscs/accessories/?area=gb&lang=en&mdl=RMT-P1BT
    addr = "FF:FF:FF:FF:FF:FF" # ILCE-xxxx
    sony_cams = [addr]

    for sony_cam in sony_cams:
        async with BleakClient(sony_cam) as client:
            # currently bleak does not have pairing support
            # therefore the device must be paired manually in OS settings
            # After that let it disconnect and turn-off autoconnect in OS
            # reboot the camera and run this script, no paring is needed next time
            # await client.pair()
            # return
            # await client.connect()
            # print("Connected with ", sony_cam)
            services = client.services.services
            handle = None
            for id, service in services.items():
                if not service.uuid == "8000ff00-ff00-ffff-ffff-ffffffffffff":
                    continue
                # print(id, service.uuid)
                chars = service.characteristics
                for char in chars:
                    if char.uuid.startswith("0000ff01"):
                        handle = char.handle

            # test shutter button to take picture
            await client.write_gatt_char(handle, shd)
            await client.write_gatt_char(handle, sfd)
            await client.write_gatt_char(handle, sfu)
            await client.write_gatt_char(handle, shu)
            # actually can subscribe to ff02 and wait for shutter complete
            await asyncio.sleep(2)
            
            ## test recording start/stop
            # await client.write_gatt_char(handle, rec)
            # await client.write_gatt_char(handle, ret)
            # await asyncio.sleep(5)
            # await client.write_gatt_char(handle, rec)
            # await client.write_gatt_char(handle, ret)
            
            # # enumberate commands with step size
            # base = bytearray(fob)
            # base = bytearray(fib)
            # for i in range(0, 0xff+1, 1):
            # # for i in range(0x10, 0xff+1, 1):
            #     base[-1] = i
            #     await client.write_gatt_char(handle, base)
            #     print(base.hex())
            #     # await asyncio.sleep(0.5) # delay a while to see the effect

            # # test commands with step size
            # base = bytearray(fob)
            # base = bytearray(fib)
            # for i in range(0, 0xff+1, 1):
            #     base[-1] = 0x0f
            #     # base[-1] = 0x70
            #     # base[-1] = 0x10
            #     await client.write_gatt_char(handle, base)
            #     print(base.hex())
            #     await asyncio.sleep(0.5) # delay a while to see the effect

            # # since the camera disconnects after receiving an invalid command
            # # we sequentially try all possible commands by passing an argument into the script
            # # scanned from 0x0100 to 0x01ff
            # # scanned from 0x020010 to 0x02ff10
            # base = bytearray(b'\x01\x00')
            # # base = bytearray(b'\x02\x00\x10')
            # base[1] += inc
            # try:
            #     print(bytes(base).hex(), end=" ")
            #     await client.write_gatt_char(handle, base)
            #     print("OK")
            # except Exception as e:
            #     print("--")

import sys
if len(sys.argv) > 1:
    # for i in {0..255}; do python ble-test.py $i; done
    inc = int(sys.argv[1])
else:
    inc = 1

asyncio.run(main(inc))
