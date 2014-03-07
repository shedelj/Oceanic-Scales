import datetime
import urllib2

now = datetime.datetime.now()

timerange = 'min_date=' + str(now.month) + str(now.day) + str(now.year - 1) + '&max_date=' + str(now.month) + str(now.day) + str(now.year)

query = 'http://www.mbari.org/lobo/cgi-bin/GetLOBOData.exe?timeunit=SPECIFY&' + timerange + '&dataquality=GOOD&station=M1SURF.TXT&x=day&y=nitrate&y=temperature&y=pH_Total_InSituT&xmin=&xmax=&ymin=&ymax=&ystack=OFF&data_format=Text&submit=SEND'

print query