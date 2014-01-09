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
ser.readline()
f = open('/tmp/logs.txt', 'w')
while 1:
    ser.write('G')
    line = ser.readline()
    f.write(line);
    reading = line.split()
    print reading
    
    now = int(time.time())-start
    temp.append([now, float(reading[1])])
    delta.append([now, (temp[-1][1]-lastTemp)/(now-lastTime)])
    power.append([now, float(reading[3])])

    lastTemp = temp[-1][1]
    lastTime = now;

    # if len(temp) > 10:
    #     temp = temp[-10:]
    #     power = power[-10:]
    #     delta = delta[-10:]

    g.plot(temp)
    h.plot(power)
    d.plot(delta)
    time.sleep(10)
