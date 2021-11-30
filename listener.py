import os
from time import sleep as sleep

with open("/dev/ttyACM0", errors="replace") as arduino:
    
    while True:
        line = arduino.readline()
        print (line)
        if line[0] == "1":
            os.system("(xdotool keydown Up; sleep 0.05; xdotool keyup Up) &")
        elif line[0] == "2":
            os.system("(xdotool keydown Up; sleep 0.35; xdotool keyup Up) &")
        #elif line == "Duck\n":
        #    os.system("(xdotool keydown Down; sleep 0.2; xdotool keyup Down) &")
