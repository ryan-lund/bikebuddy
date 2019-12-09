#!/usr/bin/env python3
import bluepy.btle as btle
p = btle.Peripheral("E8:40:BC:F6:89:3E", "random")
services=p.getServices()
for service in services:
   print(service)
s = p.getServiceByUUID("00001523-1212-efde-1523-785feabcd123")
c = s.getCharacteristics("00001525-1212-efde-1523-785feabcd123")[0]
print(c)
c.write(bytes('test', 'utf-8'))
p.disconnect()
# import struct
# from bluepy.btle import Peripheral, DefaultDelegate
# import argparse

# parser = argparse.ArgumentParser(description='Print advertisement data from a BLE device')
# parser.add_argument('addr', metavar='A', type=str, help='Address of the form XX:XX:XX:XX:XX:XX')
# args = parser.parse_args()
# addr = args.addr.lower()
# if len(addr) != 17:
#     raise ValueError("Invalid address supplied")

# DISPLAY_SERVICE_UUID = "00001523-1212-efde-1523-785feabcd123"
# DISPLAY_CHAR_UUID    = "00001525-1212-efde-1523-785feabcd123"

# addr = "E8:40:BC:F6:89:3E"

# try:
#     print("connecting")
#     bikebuddy = Peripheral(addr)

#     print("connected")

#     # Get service
#     sv = bikebuddy.getServiceByUUID(DISPLAY_SERVICE_UUID)
#     # Get characteristic
#     ch = sv.getCharacteristics(DISPLAY_CHAR_UUID)[0]

#     while True:
#         display = input("Enter a message to write to the display:\n")
#         ch.write(bytes(display, 'utf-8'))
# finally:
#     bikebuddy.disconnect()