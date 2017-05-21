import datetime
import urllib2
import csv
import StringIO
import numpy as np

temp = []
ph = []
nitrate = []

now = datetime.datetime.now()

temp_ph_query = "http://oceanview.pfeg.noaa.gov/erddap/tabledap/rtcctdRTCysirt.csv?temperature%2Cph&time%3E=2016-01-01T00%3A00%3A00Z&time%3C=2017-05-20T22%3A25%3A10Z"
temp_ph_response = urllib2.urlopen(temp_ph_query)
temp_ph = csv.reader(StringIO.StringIO(temp_ph_response.read()), delimiter=',')

nitrate_reader = csv.reader(open('nitrates.csv', 'r'), delimiter=',')

#skip the first two rows because it's just labels
temp_ph.next()
temp_ph.next()

index = 0
for row in temp_ph:
	if (index % 512 == 0):
		temp.append(float(row[0]))
		ph.append(float(row[1]))
	index += 1

index = 0
for row in nitrate_reader:
	if (index % 48 == 0):
		nitrate.append(float(row[0]))
	index += 1

max_temp = max(temp)
max_ph = max(ph)
min_temp = min(temp)
min_ph = min(ph)
max_nitrate = max(nitrate)
min_nitrate = min(nitrate) + 1

#scale variables - for each array in a below
scale = [[min_nitrate,max_nitrate],[min_temp, max_temp], [min_ph, max_ph]]
a = [nitrate, temp, ph]

print 'int lapse_size = ' + str(min(len(nitrate),len(temp))) + ';'

for i in range (0,3):
	if i == 0: print 'int nitrate_lapse[' + str(len(nitrate)) + '] = {'
	if i == 1: print 'int temperature_lapse[' + str(len(temp)) + '] = {'
	if i == 2: print 'int ph_lapse[' + str(len(ph)) + '] = {'
	s = ''
	for val in a[i]:
		s += str(int(((val - scale[i][0]) / (scale[i][1] - scale[i][0])) * 255)) + ','
	print s + '};'