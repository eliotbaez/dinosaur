#!/usr/bin/python3
import os

with open("/dev/ttyACM0", errors="replace") as arduino:
    
    while True:
        line = arduino.readline()
        print (line, end='')
        if line[0] == "1":
            os.system("(xdotool keydown Up; sleep 0.05; xdotool keyup Up) &")
        elif line[0] == "2":
            os.system("(xdotool keydown Up; sleep 0.30; xdotool keyup Up) &")
        #elif line == "Duck\n":
        #    os.system("(xdotool keydown Down; sleep 0.2; xdotool keyup Down) &")
