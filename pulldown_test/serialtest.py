
import time
import serial

v = 0
 
with open('data.txt') as fp:
	for line in fp:
		v = float(line)
		v = int(v)
		print v		

ser = serial.Serial("/dev/tty.usbmodem3a231")  # open first serial port
print ser.portstr       # check which port was really used
ser.write(chr(v)) 

time.sleep(5);
ser.write(chr(v)) 
ser.close() 
