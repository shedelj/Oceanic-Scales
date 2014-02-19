import datetime
import urllib2
from xml.dom import minidom

now = datetime.datetime.now()
#print time.strftime(time.strftime("%d/%m/%Y"))

timerange = 'min_date=' + str(now.month) + str(now.day) + str(now.year - 1) + '&max_date=' + str(now.month) + str(now.day) + str(now.year)

query = 'http://www.mbari.org/lobo/cgi-bin/GetLOBOData.exe?timeunit=SPECIFY&' + timerange + '&dataquality=GOOD&station=M1SURF.TXT&x=day&y=nitrate&y=temperature&y=pH_Total_InSituT&y=pH_Total_20C&xmin=&xmax=&ymin=&ymax=&ystack=OFF&data_format=Text&submit=SEND'

response = urllib2.urlopen(query)
html = response.read()

datafile = open('data.txt', 'w')
datafile.write(html);
datafile.close();
#xmldoc = minidom.parse(html)
#itemlist = xmldoc.getElementsByTagName('br') 
#print len(itemlist)
#print itemlist[0].attributes['name'].value
#for s in itemlist :
#    print s.attributes['name'].value



#print html
