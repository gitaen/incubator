#!/usr/bin/env python

import Gnuplot
import serial
import sys
import time

ser = serial.Serial(sys.argv[1], 115200)

temp = []
power = []
delta = []

g = Gnuplot.Gnuplot()
g.title("Temperature")
g('set style data lines')

d = Gnuplot.Gnuplot()
d.title("Delta")
d('set style data lines')

h = Gnuplot.Gnuplot()
h.title("Heater")
h('set style data lines')

start = int(time.time())
lastTemp = 0;
lastTime = start;
while 1:
    reading = ser.readline().split()
    print reading
    
    now = int(time.time())-start
    temp.append([now, float(reading[1])])
    delta.append([now, (temp[-1][1]-lastTemp)/(now-lastTime)])
    power.append([now, float(reading[3])])

    lastTemp = temp[-1][1]
    lastTime = now;

    if len(temp) > 30:
        temp = temp[-30:]
        power = power[-30:]
        delta = delta[-30:]

    g.plot(temp)
    h.plot(power)
    d.plot(delta)
