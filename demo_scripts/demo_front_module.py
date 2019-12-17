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

# FRONT_MAC, BACK_MAC = "E8:40:BC:F6:89:3E", "F9:CB:0F:79:E8:BB"
FRONT_MAC = "F5:0C:E1:0F:07:24"
# FRONT_MAC = "D3:F1:86:3C:AA:E0"
# BACK_MAC = "F9:CB:0F:79:E8:BB"

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
    # directions = get_directions('24'

    print("Connecting to Front Module...")
    front_p = connect_bikebuddy_module(FRONT_MAC)
    print("Connected to Front Module!")

    # print("Connecting to Back Module...")
    # back_p = connect_bikebuddy_module(BACK_MAC)
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

    front_light_char = front_service.getCharacteristics("00001527-1212-efde-1523-785feabcd123")[0]
    print(front_light_char)

    front_distance_char = front_service.getCharacteristics("00001528-1212-efde-1523-785feabcd123")[0]
    print(front_distance_char)

    front_back_char = front_service.getCharacteristics("00001529-1212-efde-1523-785feabcd123")[0]
    print(front_back_char)
    desc = front_back_char.getDescriptors(forUUID=0x2902)[0]
    desc.write(bytes.fromhex('0100'))

    # back_service = back_p.getServiceByUUID("00001523-1212-efde-1523-785feabcd123")
    # back_blindspot_char = back_service.getCharacteristics("00001524-1212-efde-1523-785feabcd123")[0]
    # print('Blind Spot: ' + str(back_blindspot_char))
    # desc = back_blindspot_char.getDescriptors(forUUID=0x2902)[0]
    # desc.write(bytes.fromhex('0100'))

    # back_backlight_char = back_service.getCharacteristics("00001525-1212-efde-1523-785feabcd123")[0]
    # print('Backlight: ' + str(back_backlight_char))

    # back_speeddistance_char = back_service.getCharacteristics("00001526-1212-efde-1523-785feabcd123")[0]
    # print('Speed: ' + str(back_speeddistance_char))



    # back_service = back_p.getServiceByUUID("00001523-1212-efde-1523-785feabcd123")
    # back_backlight_char = back_service.getCharacteristics("00001525-1212-efde-1523-785feabcd123")[0]
    # print(back_backlight_char)

    # front_p.setDelegate(BikeBuddyDelegate(back_backlight_char))

    try:
        while True:
            # print("waiting for notification")
            # front_ctrl_char.write(bytes.fromhex('01'))
            # time.sleep(0.1)
            # print(front_back_char.read())
            # back_backlight_char.write(front_back_char.read())

            value = input("which:\n")
            if value == 'BS':
                value = input("enter value:\n")
                front_blind_char.write(bytes.fromhex('{0}'.format(value)))
            if value == 'NAV':
                value = input("enter value:\n")
                front_nav_char.write(bytes(value, 'utf-8'))
            if value == 'DIST':
                distance_float = input("enter float:\n")
                distance_string = input("enter string: \n")
                front_distance_char.write(bytes.fromhex(float_to_hex(float(distance_float))) + bytes(distance_string, 'utf-8'))         
            if value == 'DIR':
                value = input("enter value:\n")
                front_direction_char.write(bytes.fromhex('{0}'.format(value)))
            # back_backlight_char.write(bytes.fromhex(value))
    except Exception as e:
        print(e)
    finally:
        front_p.disconnect()
        # back_p.disconnect()
    front_p.disconnect()
    # back_p.disconnect()
