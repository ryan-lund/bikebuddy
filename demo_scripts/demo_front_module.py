#!/usr/bin/env python3
import bluepy.btle as btle
import requests
import argparse
from urllib.parse import urlencode
import time
import struct
from navigation import get_directions

# FRONT_MAC, BACK_MAC = "E8:40:BC:F6:89:3E", "F9:CB:0F:79:E8:BB"
FRONT_MAC = "F5:0C:E1:0F:07:24"
# FRONT_MAC = "D3:F1:86:3C:AA:E0"
# BACK_MAC = "F9:CB:0F:79:E8:BB"

class BikeBuddyDelegate(btle.DefaultDelegate):
    def __init__(self, back_backlight_char):
        btle.DefaultDelegate.__init__(self)
        # ... initialise here
        self.back_backlight_char = back_backlight_char;

    def handleNotification(self, cHandle, data):
        # ... perhaps check cHandle
        # ... process 'data'
        print(data)
        self.back_backlight_char.write(data)
        return

def connect_bikebuddy_module(MAC_ADDR):
    return btle.Peripheral(MAC_ADDR, "random")

def float_to_hex(f):
    return hex(struct.unpack('>I', struct.pack('<f', f))[0])[2:].zfill(8)


if __name__ == "__main__":
    print("Getting directions")
    directions = get_directions('2424 Haste Street-Berkeley-CA', '2516 Bancroft Way-Berkeley-CA')

    print("Connecting to Front Module...")
    front_p = connect_bikebuddy_module(FRONT_MAC)
    print("Connected to Front Module!")


    front_service = front_p.getServiceByUUID("00001523-1212-efde-1523-785feabcd123")
    front_street_char = front_service.getCharacteristics("00001524-1212-efde-1523-785feabcd123")[0]
    print(front_street_char)

    front_direction_char = front_service.getCharacteristics("00001525-1212-efde-1523-785feabcd123")[0]
    print(front_direction_char)

    front_blind_char = front_service.getCharacteristics("00001526-1212-efde-1523-785feabcd123")[0]
    print(front_blind_char)

    front_light_char = front_service.getCharacteristics("00001527-1212-efde-1523-785feabcd123")[0]
    print(front_light_char)

    front_distance_char = front_service.getCharacteristics("00001528-1212-efde-1523-785feabcd123")[0]
    print(front_distance_char)

    front_back_char = front_service.getCharacteristics("00001529-1212-efde-1523-785feabcd123")[0]
    print(front_back_char)
    desc = front_back_char.getDescriptors(forUUID=0x2902)[0]
    desc.write(bytes.fromhex('0100'))

    for direction in directions:
        input("Press Enter for Next Direction")
        distance, distance_string, direction, street = direction[0], str(direction[0]), '0{0}'.format(str(direction[1])), direction[2]
        print(distance, distance_string, direction, street)
        front_street_char.write(bytes(street, 'utf-8'))
        front_distance_char.write(bytes.fromhex(float_to_hex(float(distance))) + bytes(distance_string, 'utf-8'))
        front_direction_char.write(bytes.fromhex(direction))

    front_p.disconnect()
