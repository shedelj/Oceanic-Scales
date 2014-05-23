
from xml.dom import minidom
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

data = open('data.txt', 'r')
html = data.read();
data.close();
print strip_tags(html)
#xmldoc = minidom.parse('data.txt')
#itemlist = xmldoc.getElementsByTagName('br') 
#print xmldoc
#print len(itemlist)
#print itemlist[0].attributes['name'].value
#for s in itemlist :
#    print s.attributes['name'].value
