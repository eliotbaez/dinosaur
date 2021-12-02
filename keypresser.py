#!/usr/bin/python3

# listener.py
# by Eliot Baez
#
# This script listens to the serial port where the Arduino is connected
# and issues system commands whenever the right signal is received. Only
# lines that begin with the number 1 or 2 will be processed, and all
# others ignored.
# 
# This script takes advantage of xdotool by Jordan Sissel
# (https://github.com/jordansissel/xdotool) to simulate keyboard input
# on systems using the X window system (https://www.x.org/wiki/).
#
# In case it isn't self explanatory, this isn't designed to be very
# portable outside of Linux based systems. Yet.

import os

with open("/dev/stdin", errors="replace") as arduino:
    
    while True:
        line = arduino.readline()
        if line[0] == "1":
            # short jump
            os.system("(xdotool keydown Up; sleep 0.05; xdotool keyup Up) &")
        elif line[0] == "2":
            # long jump
            os.system("(xdotool keydown Up; sleep 0.30; xdotool keyup Up) &")
        #elif line == "Duck\n":
        #    os.system("(xdotool keydown Down; sleep 0.2; xdotool keyup Down) &")
