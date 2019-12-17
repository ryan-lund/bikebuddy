import requests
from urllib.parse import urlencode

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
    params = {
        'start': start_coords,
        'end': end_coords,
    }
    call = requests.get('https://api.openrouteservice.org/v2/directions/cycling-regular?api_key=5b3ce3597851110001cf6248cbd8b98cf94b49998c93ca0605780f15&{0}'.format(urlencode(params)), headers=HEADERS)
    directions = call.json()
    turn_by_turn = []
    for step in directions['features'][0]['properties']['segments'][0]['steps']:
        turn_by_turn.append([float(step['distance']), parse_nav_direction(step['instruction']), replace_with_abbreviation(parse_nav_waypoint(step['instruction']))])
    return turn_by_turn

def parse_nav_direction(instruction):
    if 'Continue' in instruction or 'Head' in instruction or 'Keep' in instruction:
        return 0
    elif 'Turn' in instruction:
        if 'left' in instruction:
            return 1
        elif 'right' in instruction:
            return 2
    elif 'Arrive' in instruction:
        return 3
    else:
        return 4

def parse_nav_waypoint(instruction):
    if 'Arrive at' in instruction:
        return 'Arrived!' 
    elif ' on ' in instruction:
        return instruction.split(' on ')[1]
    elif ' onto ' in instruction:
        return instruction.split(' onto ')[1]
    else:
        return ''

abbreviations = {'Drive': 'Dr', 'Boulevard': 'Blvd', 'Street': 'St', 'Avenue':'Ave', 'Road':' Rd'}
def replace_with_abbreviation(waypoint):
    if waypoint:
        split_waypoint = waypoint.split()
        waypoint_type = split_waypoint[-1]
        if waypoint_type in abbreviations.keys():
            return ' '.join(split_waypoint[:-1]+[abbreviations[waypoint_type]])
        else:
            return waypoint
    else:
        return waypoint