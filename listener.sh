#!/bin/bash

# mimic the TTY settings from /usr/bin/screen, primarily for cosmetics:
# set the baudrate to 19200 for the serial monitor before opening
# -onlcr : do not translate newlines to carriage returns on output
# -opost : do not postprocess output
# -icrnl : do not translate carriage return to newlines on input
# -isig  : disable interrupt, quit, and suspend characters
# -icanon : disable erase, kill, werase, rprnt characters
# -echo  : do not echo input characters
stty --file=/dev/ttyACM0 19200 brkint -onlcr -opost -icrnl -isig -icanon -echo min 100 time 2

# then read the contents of the file to the listener script, while also
# duplicating the output to stderr
cat /dev/ttyACM0 | tee /dev/stderr | python3 keypresser.py
