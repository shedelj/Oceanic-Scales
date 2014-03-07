import datetime
import urllib2

from HTMLParser import HTMLParser

class MLStripper(HTMLParser):
    def __init__(self):
        self.reset()
        self.fed = []
    def handle_data(self, d):
        self.fed.append(d)
    def get_data(self):
        return ''.join(self.fed)

def strip_tags(html):
    s = MLStripper()
    s.feed(html)
    return s.get_data()


now = datetime.datetime.now()

timerange = 'min_date=' + str(now.month) + str(now.day) + str(now.year - 1) + '&max_date=' + str(now.month) + str(now.day) + str(now.year)

query = 'http://www.mbari.org/lobo/cgi-bin/GetLOBOData.exe?timeunit=SPECIFY&' + timerange + '&dataquality=GOOD&station=L02SURF.TXT&x=day&y=nitrate&y=temperature&y=fluorescence&xmin=&xmax=&ymin=&ymax=&ystack=OFF&data_format=Text&submit=SEND'

response = urllib2.urlopen(query)
html = response.read()


import StringIO
import csv

f = StringIO.StringIO(html)
reader = csv.reader(f, delimiter=',')


buf = [[-1, -1, 0], [-1, -1, 0], [-1, -1, 0]]

#nmin = -1
#nmax = -1
#nprev = 0

#tmin = -1
#tmax = -1
#tprev = 0

#pmin = -1
#pmax = -1
#pprev = 0

a = [[],[], []]
index = 0
for row in reader:
    #print '\t'.join(row)
    if len(row) != 5:
        continue
    if index % 24 is not 0:
        index+= 1
        continue
    for i in range(2,5):
        val = float(strip_tags(row[i]))
        diff = buf[i-2][1] - buf[i-2][0]
        if val > 500000:
            val = buf[i-2][2]
        if diff != 0 and (val > buf[i-2][1] + diff / 2 or val < buf[i-2][0] - diff / 2):
            val = buf[i-2][2]
        #print val
        a[i-2].append(val)
        if buf[i - 2][0] == -1: 
            buf[i - 2][0] = val
        if buf[i - 2][1] == -1:
            buf[i - 2][1] = val
        buf[i-2][0] = min(buf[i-2][0], val)
        buf[i-2][1] = max(buf[i-2][1], val)
    index +=1

print 'min is ' + str(buf[0][0])
print 'max is ' + str(buf[0][1])
print 'void init_vals(){'

for i in range(0,3):
    index = 0
    if i == 0: label = '   nitrate_lapse['
    if i == 1: label = '   temperature_lapse['
    if i == 2: label = '   ph_lapse['
    for val in a[i]:
        print label + str(index) + '] = ' + str(int(((val - buf[i][0]) / (buf[i][1] - buf[i][0])) * 255)) + ';'
        index += 1

print '}'

#
#datafile = open('data.txt', 'w')
#datafile.write(html);
#datafile.close();
#xmldoc = minidom.parse(html)
#itemlist = xmldoc.getElementsByTagName('br') 
#print len(itemlist)
#print itemlist[0].attributes['name'].value
#for s in itemlist :
#    print s.attributes['name'].value



#print html
