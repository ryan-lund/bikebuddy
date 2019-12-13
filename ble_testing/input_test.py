import argparse

parser = argparse.ArgumentParser(description='Print advertisement data from a BLE device')
parser.add_argument('start', metavar='A', type=str, help='Address of the form Street-City-State')
parser.add_argument('end', metavar='A', type=str, help='Address of the form Street-City-State')
args = parser.parse_args()

start_address, start_locality, start_region = args.start.split('-')
end_address, end_locality, end_region = args.end.split('-')

headers = {
    'Accept': 'application/json, application/geo+json, application/gpx+xml, img/png; charset=utf-8',
}

start_params = {
    'address': ,
    'locality': ,
}
