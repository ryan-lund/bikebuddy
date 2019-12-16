#!/usr/bin/env python3
import bluepy.btle as btle
import requests
import argparse
from urllib.parse import urlencode
import time
import struct

HEADERS = {
    'Accept': 'application/json, application/geo+json, application/gpx+xml, img/png; charset=utf-8',
}

FRONT_MAC, BACK_MAC = "E8:40:BC:F6:89:3E", "F9:CB:0F:79:E8:BB"


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

class BikeBuddyDelegate(btle.DefaultDelegate):
    def __init__(self):
        btle.DefaultDelegate.__init__(self)
        # ... initialise here

    def handleNotification(self, cHandle, data):
        # ... perhaps check cHandle
        # ... process 'data'
        print(data)
        return

def connect_bikebuddy_module(MAC_ADDR):
    return btle.Peripheral(MAC_ADDR, "random")

if __name__ == "__main__":


    print("Connecting to Front Module...")
    front_p = connect_bikebuddy_module(FRONT_MAC)
    front_p.setDelegate(BikeBuddyDelegate())
    print("Connected to Front Module!")

    # print("Connecting to Back Module...")
    # back_p = connect_bikebuddy_module(BACK_MAC,)
    # print("Connected to Back Module!")
    # Set up services and characteristics

    front_service = front_p.getServiceByUUID("00001523-1212-efde-1523-785feabcd123")
    front_nav_char = front_service.getCharacteristics("00001524-1212-efde-1523-785feabcd123")[0]
    print(front_nav_char)
    # desc = front_back_ctrl_char.getDescriptors(forUUID=0x2902)[0]
    # desc.write(bytes.fromhex('0100'))

    front_direction_char = front_service.getCharacteristics("00001525-1212-efde-1523-785feabcd123")[0]
    print(front_direction_char)

    front_blind_char = front_service.getCharacteristics("00001526-1212-efde-1523-785feabcd123")[0]
    print(front_blind_char)
    desc = front_blind_char.getDescriptors(forUUID=0x2902)[0]
    desc.write(bytes.fromhex('0100'))

    front_light_char = front_service.getCharacteristics("00001527-1212-efde-1523-785feabcd123")[0]
    print(front_light_char)


    # byte allocation distance (4), direction (1), rest (15)


    front_nav_char.write(bytes.fromhex('01'))
    front_nav_char.write(bytes.fromhex('00'))
    front_direction_char.write(bytes.fromhex('01'))
    front_direction_char.write(bytes.fromhex('00'))
    front_light_char.write(bytes.fromhex('01'))
    front_light_char.write(bytes.fromhex('00'))
    try:
        while True:
            print("waiting")
            # front_ctrl_char.write(bytes.fromhex('01'))
            while front_p.waitForNotifications(0.5):
                print("got notification")
            value = input("Enter value to module:\n")
            # # print(bytes.fromhex(float_to_hex(float(value))))
            # # front_ctrl_char.write(bytes.fromhex(float_to_hex(float(value))))
            front_nav_char.write(bytes.fromhex('{0}'.format(value)))
    finally:
        front_p.disconnect()
