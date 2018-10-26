# JEFFREY LAKEMAN @ 11-10-2018
# Python 2.7.13
# Downloads data from the thingspeak server

# Import libraries
import urllib2
import json

# Declare required thingspeak connection variables
# Replace values with your own values
READ_API_KEY = 'YOUR_READ_API_KEY'
CHANNEL_ID = 'YOUR_CHANNEL_ID'

# Connect and download data
def tsdownload():
    conn = urllib2.urlopen('http://api.thingspeak.com/channels/%s/feeds/last.json?api_key=%s' % (CHANNEL_ID, READ_API_KEY))
    response = conn.read()
    data = json.loads(response)
    return(data['field1'])
