#!/usr/bin/env python

import Gnuplot
import serial
import sys
import time

ser = serial.Serial(sys.argv[1], 115200)

temp = []
power = []

g = Gnuplot.Gnuplot()
g.title("Temperature")
g('set style data lines')

h = Gnuplot.Gnuplot()
h.title("Heater")
h('set style data lines')

start = int(time.time())

while 1:
    reading = ser.readline().split()
    print reading
    
    now = int(time.time())-start
    temp.append([now, reading[1]])
    power.append([now, reading[3]])

    if len(temp) > 60:
        temp = temp[-60:]
        power = power[-60:]

    g.plot(temp)
    h.plot(power)
