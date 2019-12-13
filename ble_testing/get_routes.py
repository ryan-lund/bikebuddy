# General Info:

# The expected order for all coordinates arrays is [lon, lat]
# All timings are in seconds
# All distances are in meters
# A time_window object is a pair of timestamps in the form [start, end]
import requests
from urllib.parse import urlencode

headers = {
    'Accept': 'application/json, application/geo+json, application/gpx+xml, img/png; charset=utf-8',
}
address_start = '2424 Haste Street'
locality_start = 'Berkeley'
params_start = {
    'address': address_start,
    'locality': locality_start,
}
call = requests.get('https://api.openrouteservice.org/geocode/search/structured?api_key=5b3ce3597851110001cf6248cbd8b98cf94b49998c93ca0605780f15&{0}'.format(urlencode(params_start)), headers=headers)

# print(call.status_code, call.reason)
# print(call.text)

geo1 = call.json()

address_end = '4617 Korbel Street'
locality_end = 'Union City'
params_end = {
    'address': address_end,
    'locality': locality_end,
}
call = requests.get('https://api.openrouteservice.org/geocode/search/structured?api_key=5b3ce3597851110001cf6248cbd8b98cf94b49998c93ca0605780f15&{0}'.format(urlencode(params_end)), headers=headers)

# print(call.status_code, call.reason)
# print(call.text)

geo2 = call.json()

start = geo1['features'][0]['geometry']['coordinates']
end = geo2['features'][0]['geometry']['coordinates']

start_coords = ','.join([str(start[0]),str(start[1])])
end_coords = ','.join([str(end[0]),str(end[1])])

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