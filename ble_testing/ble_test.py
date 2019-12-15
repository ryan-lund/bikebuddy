#!/usr/bin/env python3
import bluepy.btle as btle
import requests
import argparse
from urllib.parse import urlencode
import time
import struct


# parser = argparse.ArgumentParser(description='Print advertisement data from a BLE device')
# parser.add_argument('start', metavar='A', type=str, help='Address of the form Street-City-State')
# parser.add_argument('end', metavar='A', type=str, help='Address of the form Street-City-State')
# args = parser.parse_args()


HEADERS = {
    'Accept': 'application/json, application/geo+json, application/gpx+xml, img/png; charset=utf-8',
}

def get_geocode(location):
    address, city, state = location.split('-')
    params = {
        'address': address,
        'locality': city,
        'region': state,
    }
    call = requests.get('https://api.openrouteservice.org/geocode/search/structured?api_key=5b3ce3597851110001cf6248cbd8b98cf94b49998c93ca0605780f15&{0}'.format(urlencode(params)), headers=HEADERS)
    coordinates = call.json()['features'][0]['geometry']['coordinates']
    coordinates = ','.join([str(coordinates[0]),str(coordinates[1])])
    return coordinates

def get_directions(start, end):
    start_coords = get_geocode(start)
    end_coords = get_geocode(end)
    print(start_coords, end_coords)
    params = {
        'start': start_coords,
        'end': end_coords,
    }
    call = requests.get('https://api.openrouteservice.org/v2/directions/driving-car?api_key=5b3ce3597851110001cf6248cbd8b98cf94b49998c93ca0605780f15&{0}'.format(urlencode(params)), headers=HEADERS)
    directions = call.json()
    turn_by_turn = []
    for step in directions['features'][0]['properties']['segments'][0]['steps']:
        turn_by_turn.append("{0}m {1}".format(step['distance'],step['instruction']))
    return turn_by_turn

class MyDelegate(btle.DefaultDelegate):
    def __init__(self):
        btle.DefaultDelegate.__init__(self)
        # ... initialise here

    def handleNotification(self, cHandle, data):
        # ... perhaps check cHandle
        # ... process 'data'
        print(data)

FRONT_MAC, BACK_MAC = "E8:40:BC:F6:89:3E", "F9:CB:0F:79:E8:BB"

if __name__ == "__main__":
    def connect_bikebuddy_module(MAC_ADDR):
        return btle.Peripheral(MAC_ADDR, "random")

    print("Connecting to Front Module...")
    front_p = connect_bikebuddy_module(FRONT_MAC)
    front_p.setDelegate(MyDelegate())
    print("Connected to Front Module!")

    # print("Connecting to Back Module...")
    # back_p = connect_bikebuddy_module(BACK_MAC,)
    # print("Connected to Back Module!")
    # Set up services and characteristics

    front_service = front_p.getServiceByUUID("00001523-1212-efde-1523-785feabcd123")
    front_back_ctrl_char = front_service.getCharacteristics("00001524-1212-efde-1523-785feabcd123")[0]
    print(front_back_ctrl_char)
    desc = front_back_ctrl_char.getDescriptors(forUUID=0x2902)[0]
    desc.write(bytes.fromhex('0100'))

    front_back_ctrl_handle = front_back_ctrl_char.getHandle()
    front_ctrl_char = front_service.getCharacteristics("00001525-1212-efde-1523-785feabcd123")[0]
    print(front_ctrl_char)



    # distance (4), direction (1)
    def float_to_hex(f):
        return hex(struct.unpack('<I', struct.pack('<f', f))[0])[2:]


    front_ctrl_char.write(bytes.fromhex('01'))
    front_ctrl_char.write(bytes.fromhex('05'))
    try:
        while True:
            print("waiting")
            # front_ctrl_char.write(bytes.fromhex('01'))
            while front_p.waitForNotifications(3.0):
                print("got notification")
        #     value = input("Enter value to module:\n")
        #     # # print(bytes.fromhex(float_to_hex(float(value))))
        #     # # front_ctrl_char.write(bytes.fromhex(float_to_hex(float(value))))
        #     front_ctrl_char.write(bytes.fromhex('{0}'.format(value)))
    finally:
        front_p.disconnect()
