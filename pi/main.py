#!/usr/bin/env python2

# JEFFREY LAKEMAN @ 13-10-2018
# Python 2.7.13
# Collects ts data and displays it on a python tornado webserver
# Also has functionality for a flashing led if the sensor data becomes too high
# For use on a Raspberry Pi

# Import libraries
from threading import Thread
from tsdownload import tsdownload
import tornado.ioloop
import tornado.web
from time import sleep
import RPi.GPIO as gpio

# Initialise GPIO pin
gpio.setwarnings(False)
gpio.setmode(gpio.BCM)
gpio.setup(4, gpio.OUT, initial=gpio.LOW)

# Declare variables
value = 0
temp = 0
max_temp = 30
led = False

running = False

def get_temp():
    # This function runs on its own thread; continuously downloading data from the thingspeak server and providing it to the webserver
    # It also activates the flashing led should the data increase above a set value
    global value
    global temp
    global max_temp
    global led
    global running

    running = True

    while running:
        value = value + 1
        if value % 10 == 0:
            temp = float(tsdownload())
            print(temp)

        if temp >= max_temp:
            led = not led
            if led:
                gpio.output(4, gpio.HIGH)
            else:
                gpio.output(4, gpio.LOW)
        else:
            led = False
            gpio.output(4, gpio.LOW)
        sleep(0.5)

# Create the main page
class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.render("index.html", title="temp", temp=temp)

def make_app():
    return tornado.web.Application([
        (r"/", MainHandler),
    ])

# Start second thread, then open the webserver
if __name__ == "__main__":
    thread = Thread(target = get_temp)
    thread.start()

    app = make_app()
    app.listen(16686)
    loop = tornado.ioloop.IOLoop.current()

    # Closes the webserver and the second thread on keyboard interrupt
    try:
        print("Server starting...")
        loop.start()
    except KeyboardInterrupt:
        pass
    finally:
        loop.stop()
        loop.close(True)
        running = False
    print("Server shut down")
