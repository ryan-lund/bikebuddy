#!/usr/bin/env python3
import bluepy.btle as btle
import requests
import argparse
from urllib.parse import urlencode

# headers = {
#     'Accept': 'application/json, application/geo+json, application/gpx+xml, img/png; charset=utf-8',
# }

# parser = argparse.ArgumentParser(description='Print advertisement data from a BLE device')
# parser.add_argument('start', metavar='A', type=str, help='Address of the form Street-City-State')
# parser.add_argument('end', metavar='A', type=str, help='Address of the form Street-City-State')
# args = parser.parse_args()

# start_address, start_locality, start_region = args.start.split('-')
# end_address, end_locality, end_region = args.end.split('-')

# start_params = {
#     'address': start_address,
#     'locality': start_locality,
#     'region': start_region,
# }
# call = requests.get('https://api.openrouteservice.org/geocode/search/structured?api_key=5b3ce3597851110001cf6248cbd8b98cf94b49998c93ca0605780f15&{0}'.format(urlencode(start_params)), headers=headers)

# print(call.status_code, call.reason)
# print(call.text)

# start_geo_json = call.json()

# end_params = {
#     'address': end_address,
#     'locality': end_locality,
#     'region': end_region,
# }
# call = requests.get('https://api.openrouteservice.org/geocode/search/structured?api_key=5b3ce3597851110001cf6248cbd8b98cf94b49998c93ca0605780f15&{0}'.format(urlencode(end_params)), headers=headers)

# # print(call.status_code, call.reason)
# # print(call.text)

# end_geo_json = call.json()

# start_geo = start_geo_json['features'][0]['geometry']['coordinates']
# end_geo = end_geo_json['features'][0]['geometry']['coordinates']

# start_coords = ','.join([str(start_geo[0]),str(start_geo[1])])
# end_coords = ','.join([str(end_geo[0]),str(end_geo[1])])

# params_directions = {
#     'start': start_coords,
#     'end': end_coords,
# }

# call = requests.get('https://api.openrouteservice.org/v2/directions/driving-car?api_key=5b3ce3597851110001cf6248cbd8b98cf94b49998c93ca0605780f15&{0}'.format(urlencode(params_directions)), headers=headers)

# # print(call.status_code, call.reason)
# # print(call.text)
# directions = call.json()
# turn_by_turn = []
# for step in directions['features'][0]['properties']['segments'][0]['steps']:
#     turn_by_turn.append("{0}m {1}".format(step['distance'],step['instruction']))

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

# print(bytes("{:<32}".format('whoa it works'), 'utf-8'))
front_ctrl_char.write(bytes.fromhex('01'))
# for turn in turn_by_turn[:3]:
    # front_ctrl_char.write(bytes("{:<32}".format(turn), 'utf-8'))
for _ in range(5):
    back_ctrl_state = front_back_ctrl_char.read()
    # print(back_ctrl_state)
    back_ctrl_char.write(back_ctrl_state)
front_ctrl_char.write(bytes.fromhex('00'))
for _ in range(5):
    back_ctrl_state = front_back_ctrl_char.read()
    # print(back_ctrl_state)
    back_ctrl_char.write(back_ctrl_state)
    
front_p.disconnect()
back_p.disconnect()