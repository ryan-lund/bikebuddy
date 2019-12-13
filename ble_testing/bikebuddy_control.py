#!/usr/bin/env python3
import bluepy.btle as btle

front_mac, back_mac = "E8:40:BC:F6:89:3E", "F9:CB:0F:79:E8:BB"
print("Connecting to Front Module...")
front_p = btle.Peripheral(front_mac, "random")
print("Connected to Front Module!")
print("Connecting to Back Module...")
back_p = btle.Peripheral(back_mac, "random")
print("Connected to Back Module!")


# Set up services and characteristics
front_service = front_p.getServiceByUUID("00001523-1212-efde-1523-785feabcd123")
front_back_ctrl_char = front_service.getCharacteristics("00001524-1212-efde-1523-785feabcd123")[0]
print(front_back_ctrl_char)
front_ctrl_char = front_service.getCharacteristics("00001525-1212-efde-1523-785feabcd123")[0]
print(front_ctrl_char)
# c_led.write(bytes("{:<32}".format('1'), 'utf-8'))

back_service = back_p.getServiceByUUID("00001523-1212-efde-1523-785feabcd123")
back_ctrl_char = back_service.getCharacteristics("00001525-1212-efde-1523-785feabcd123")[0]
print(back_ctrl_char)

front_ctrl_char.write(bytes.fromhex('01'))
for _ in range(5):
    back_ctrl_state = front_back_ctrl_char.read()
    print(back_ctrl_state)
    back_ctrl_char.write(back_ctrl_state)
front_ctrl_char.write(bytes.fromhex('00'))
for _ in range(5):
    back_ctrl_state = front_back_ctrl_char.read()
    print(back_ctrl_state)
    back_ctrl_char.write(back_ctrl_state)
front_p.disconnect()
back_p.disconnect()

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