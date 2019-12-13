# General Info:

# The expected order for all coordinates arrays is [lon, lat]
# All timings are in seconds
# All distances are in meters
# A time_window object is a pair of timestamps in the form [start, end]
import requests
import argparse
from urllib.parse import urlencode

headers = {
    'Accept': 'application/json, application/geo+json, application/gpx+xml, img/png; charset=utf-8',
}

parser = argparse.ArgumentParser(description='Print advertisement data from a BLE device')
parser.add_argument('start', metavar='A', type=str, help='Address of the form Street-City-State')
parser.add_argument('end', metavar='A', type=str, help='Address of the form Street-City-State')
args = parser.parse_args()

start_address, start_locality, start_region = args.start.split('-')
end_address, end_locality, end_region = args.end.split('-')

start_params = {
    'address': start_address,
    'locality': start_locality,
    'region': start_region,
}
call = requests.get('https://api.openrouteservice.org/geocode/search/structured?api_key=5b3ce3597851110001cf6248cbd8b98cf94b49998c93ca0605780f15&{0}'.format(urlencode(start_params)), headers=headers)

print(call.status_code, call.reason)
print(call.text)

start_geo_json = call.json()

end_params = {
    'address': end_address,
    'locality': end_locality,
    'region': end_region,
}
call = requests.get('https://api.openrouteservice.org/geocode/search/structured?api_key=5b3ce3597851110001cf6248cbd8b98cf94b49998c93ca0605780f15&{0}'.format(urlencode(end_params)), headers=headers)

# print(call.status_code, call.reason)
# print(call.text)

end_geo_json = call.json()

start_geo = start_geo_json['features'][0]['geometry']['coordinates']
end_geo = end_geo_json['features'][0]['geometry']['coordinates']

start_coords = ','.join([str(start_geo[0]),str(start_geo[1])])
end_coords = ','.join([str(end_geo[0]),str(end_geo[1])])

params_directions = {
    'start': start_coords,
    'end': end_coords,
}

call = requests.get('https://api.openrouteservice.org/v2/directions/driving-car?api_key=5b3ce3597851110001cf6248cbd8b98cf94b49998c93ca0605780f15&{0}'.format(urlencode(params_directions)), headers=headers)

# print(call.status_code, call.reason)
# print(call.text)
directions = call.json()

for step in directions['features'][0]['properties']['segments'][0]['steps']:
    print("{0} m".format(step['distance']),step['instruction'])