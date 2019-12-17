#!/usr/bin/env python3
import bluepy.btle as btle
import requests
import argparse
from urllib.parse import urlencode
import time
import struct
import threading
import logging
from navigation import get_directions

# FRONT_MAC, BACK_MAC = "E8:40:BC:F6:89:3E", "F9:CB:0F:79:E8:BB"
FRONT_MAC = "F5:0C:E1:0F:07:24"
# BACK_MAC = "D3:F1:86:3C:AA:E0"
BACK_MAC = "F9:CB:0F:79:E8:BB"


def float_to_hex(f):
    return hex(struct.unpack('>I', struct.pack('<f', f))[0])[2:].zfill(8)

def hex_to_float(h):
    return struct.unpack('>f', h)[0].round(2)

def hex_to_int(h):
    return struct.unpack('>i', h)[0]

class BikeBuddyBLE():
    def __init__(self, front_mac, back_mac):
        self.front_mac = front_mac
        self.back_mac = back_mac
        print("Connecting to Bike Buddy Front and Back...")
        self.back_p = self.connect_module(back_mac)
        self.front_p = self.connect_module(front_mac)
        print("Connected!")
        self.front_service = self.front_p.getServiceByUUID("00001523-1212-efde-1523-785feabcd123")
        self.front_street_char = self.front_service.getCharacteristics("00001524-1212-efde-1523-785feabcd123")[0]
        print(self.front_street_char)

        self.front_direction_char = self.front_service.getCharacteristics("00001525-1212-efde-1523-785feabcd123")[0]
        print('Front Direction: ' + str(self.front_direction_char))

        self.front_blind_char = self.front_service.getCharacteristics("00001526-1212-efde-1523-785feabcd123")[0]
        print('Front Blind Spot: ' + str(self.front_blind_char))

        self.front_light_char = self.front_service.getCharacteristics("00001527-1212-efde-1523-785feabcd123")[0]
        print('Front Light: ' + str(self.front_light_char))

        self.front_distance_char = self.front_service.getCharacteristics("00001528-1212-efde-1523-785feabcd123")[0]
        print('Front Distance: ' + str(self.front_distance_char))

        self.front_backlight_char = self.front_service.getCharacteristics("00001529-1212-efde-1523-785feabcd123")[0]
        print('Front to Back Light: ' + str(self.front_backlight_char))

        self.back_service = self.back_p.getServiceByUUID("00001523-1212-efde-1523-785feabcd123")
        self.back_blindspot_char = self.back_service.getCharacteristics("00001524-1212-efde-1523-785feabcd123")[0]
        print('Back Blind Spot: ' + str(self.back_blindspot_char))

        self.back_backlight_char = self.back_service.getCharacteristics("00001525-1212-efde-1523-785feabcd123")[0]
        print('Back Light: ' + str(self.back_backlight_char))

        self.back_speed_char = self.back_service.getCharacteristics("00001526-1212-efde-1523-785feabcd123")[0]
        print('Speed: ' + str(self.back_speed_char))



    def connect_module(self, mac_addr):
        return btle.Peripheral(mac_addr, "random")

def polling_back_module(bikebuddy):
    try:
        while True:
            time.sleep(0.5)
            blindspot_val = bikebuddy.back_blindspot_char.read()
            print('Blind Spot Read: ' + str(blindspot_val.hex()))
            bikebuddy.front_blind_char.write(blindspot_val)
            time.sleep(0.5)
            backlight_val = bikebuddy.front_backlight_char.read()
            print('Tail Light Read: ' + str(backlight_val.hex()))
            bikebuddy.back_backlight_char.write(backlight_val)
    except Exception as e:
        print(e)
        raise e
    finally:
        return 

def polling_front_module(bikebuddy):
    try:
        i = 15
        while i > 0:
            # time.sleep(0.3)
            # backlight_val = bikebuddy.front_backlight_char.read()
            # print('Tail Light Read: ' + str(backlight_val.hex()))
            # bikebuddy.back_backlight_char.write(backlight_val)
            i -= 1
    except Exception as e:
        print(e)
        raise e
    finally:
        return 


if __name__ == "__main__":
    try:
        print("Getting directions")
        # directions = get_directions('2424 Haste Street-Berkeley-CA', '4617 Korbel Street-Union City-CA')
        # for direction in directions:
        #     print(direction)

        bikebuddy = BikeBuddyBLE(FRONT_MAC, BACK_MAC)

        format = "%(asctime)s: %(message)s"
        logging.basicConfig(format=format, level=logging.INFO,
                            datefmt="%H:%M:%S")

        logging.info("Main    : create and start back thread %d.", 1)
        back = threading.Thread(target=polling_back_module, args=([bikebuddy]))
        back.start()

        logging.info("Main    : create and start front thread %d.", 2)
        front = threading.Thread(target=polling_front_module, args=([bikebuddy]))
        front.start()

        back.join()
        front.join()
    finally:
        bikebuddy.front_p.disconnect()
        bikebuddy.back_p.disconnect()
