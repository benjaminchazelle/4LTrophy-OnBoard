#!/usr/bin/python3

#################################################################
# stepper.py - Stepper motor control using Gertbot and Python #
# Based on Gertbot example code at http://www.gertbot.com/ #
#################################################################


import time
import random
import sys

import math

i = 0.0
j = 0.0
k = 0.0
l = 0.0

gpslat = 48.8566140
gpslng = 2.3522219


while 1==1:
	
	print "ADC0:"+str(math.sin(i)*4)
	print "ADC1:"+str(math.sin(j)*4)
	print "ADC2:"+str(math.sin(k)*4)
	print "ADC3:"+str(math.sin(l)*4)
	print "GPSLat:"+str(gpslat)
	print "GPSLng:"+str(gpslng)
	print ""
	sys.stdout.flush()
	
	i += 0.1
	j += 0.05
	k += 0.02
	l += 0.01
	
	gpslat -= 0.0002
	
	time.sleep(60)
