


import serial

ser = serial.Serial("/dev/tty.usbmodem3a231")  # open first serial port
print ser.portstr       # check which port was really used
