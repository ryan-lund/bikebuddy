#!/usr/bin/env python3
import bluepy.btle as btle
import requests
import argparse
from urllib.parse import urlencode
import time
import struct
from navigation import get_directions
import threading

# FRONT_MAC, BACK_MAC = "E8:40:BC:F6:89:3E", "F9:CB:0F:79:E8:BB"
FRONT_MAC = "F5:0C:E1:0F:07:24"
# FRONT_MAC = "D3:F1:86:3C:AA:E0"
BACK_MAC = "F9:CB:0F:79:E8:BB"

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

def hex_to_float(h):
    return struct.unpack('<f', h)[0]

def hex_to_int(h):
    return struct.unpack('<i', h)[0]

def loop_fn():
    while True:
        time.sleep(0.1)
        front_light_states = front_back_char.read()
        back_backlight_char.write(front_light_states)
        light_states = front_light_states[0]
        back_blindspot_states = back_blindspot_char.read()
        front_blind_char.write(back_blindspot_states)
        blindspot_states = back_blindspot_states[0]
        speed_distance = back_speeddistance_char.read()
        print('Speed: ' + str(round(hex_to_float(speed_distance[:4]),2)))
        speed_string = str(round(hex_to_float(speed_distance[:4]),2)).ljust(4,'0')
        front_speed_char.write(bytes(speed_string, 'utf-8'))
        left_blindspot, right_blindspot = (blindspot_states & 1), (blindspot_states & 2) >> 1
        print("Left Blind: {0}, Right Blind: {1}".format(left_blindspot, right_blindspot))
        brake, left, right, tail = (light_states & 1), (light_states & 2) >> 1, (light_states & 4) >> 2, (light_states & 8) >> 3
        print('Brake: {0}, Left: {1}, Right: {2}, Tail: {3}'.format(brake, left, right, tail))


if __name__ == "__main__":
    print("Getting directions")
    start_addr = '2424 Haste Street-Berkeley-CA'
    end_addr = '2516 Bancroft Way-Berkeley-CA'
    directions = get_directions(start_addr, end_addr)

    print("Connecting to Front Module...")
    front_p = connect_bikebuddy_module(FRONT_MAC)
    print("Connected to Front Module!")

    front_service = front_p.getServiceByUUID("00001523-1212-efde-1523-785feabcd123")
    front_street_char = front_service.getCharacteristics("00001524-1212-efde-1523-785feabcd123")[0]
    print('Street: ' + str(front_street_char))

    front_direction_char = front_service.getCharacteristics("00001525-1212-efde-1523-785feabcd123")[0]
    print('Direction: ' + str(front_direction_char))

    front_blind_char = front_service.getCharacteristics("00001526-1212-efde-1523-785feabcd123")[0]
    print('Blind Spot Front: ' + str(front_blind_char))

    front_speed_char = front_service.getCharacteristics("00001527-1212-efde-1523-785feabcd123")[0]
    print('Front Speed' + str(front_speed_char))

    front_distance_char = front_service.getCharacteristics("00001528-1212-efde-1523-785feabcd123")[0]
    print('Distance' + str(front_distance_char))

    front_back_char = front_service.getCharacteristics("00001529-1212-efde-1523-785feabcd123")[0]
    print('Front to Back Light: ' + str(front_back_char))
    desc = front_back_char.getDescriptors(forUUID=0x2902)[0]
    desc.write(bytes.fromhex('0100'))

    print("Connecting to Back Module...")
    back_p = connect_bikebuddy_module(BACK_MAC)
    print("Connected to Back Module!")

    back_service = back_p.getServiceByUUID("00001523-1212-efde-1523-785feabcd123")
    back_blindspot_char = back_service.getCharacteristics("00001524-1212-efde-1523-785feabcd123")[0]
    print('Blind Spot Back: ' + str(back_blindspot_char))
    desc = back_blindspot_char.getDescriptors(forUUID=0x2902)[0]
    desc.write(bytes.fromhex('0100'))

    back_backlight_char = back_service.getCharacteristics("00001525-1212-efde-1523-785feabcd123")[0]
    print('Back light: ' + str(back_backlight_char))

    back_speeddistance_char = back_service.getCharacteristics("00001526-1212-efde-1523-785feabcd123")[0]
    print('Speed: ' + str(back_speeddistance_char))

    try:
        x = threading.Thread(target=loop_fn)
        x.start()

        for direction in directions:
            input("Press Enter for Next Direction")
            distance, distance_string, direction, street = direction[0], str(direction[0]).ljust(6,'0'), '0{0}'.format(str(direction[1])), direction[2]
            print(distance, distance_string, direction, street)
            front_street_char.write(bytes(street, 'utf-8'))
            front_distance_char.write(bytes.fromhex(float_to_hex(float(distance))) + bytes(distance_string, 'utf-8'))
            front_direction_char.write(bytes.fromhex(direction)) 
    finally:
        front_p.disconnect()
        back_p.disconnect()        
    front_p.disconnect()
    back_p.disconnect()
