'''
Created on Oct 13, 2010

@author: kblaszcz
'''
from django.template.loader import render_to_string
import datetime
import re

class DateOption(object):

    def __init__(self, userfriendlyname, name, template, stdval = 0):
        self.name = name
        self.userfriendlyname = userfriendlyname
        self.valueexpr =  r'(?P<day>[0-9]{2}).(?P<month>[0-9]{2}).(?P<year>[0-9]{4})_(?P<hour>[0-9]{2}):(?P<minute>[0-9]{2}):(?P<second>[0-9]{2})'
        
        self.fullexpression = r'.*?' + self.name+"="+self.valueexpr + r'.*?'
        self.stdseconddiff = stdval;
        self.template = template
        
    def getFullExpression(self):
        return self.fullexpression 
    
    def getValueExpression(self):
        return r'.*?' + self.valueexpr + r'.*?'
        
    def getName(self):
        return self.name
    
    def getCorrectedString(self, value, options):
        valuedict = re.match(self.fullexpression, options).groupdict() 
        correctedoptions = []
        
        correctedoptions.append(self.name)
        correctedoptions.append("=")
        correctedoptions.append('%02d' % (int(valuedict['day'])) )
        correctedoptions.append('.')
        correctedoptions.append('%02d' % (int(valuedict['month'])) )
        correctedoptions.append('.')
        correctedoptions.append('%04d' % (int(valuedict['year'])) )
        correctedoptions.append('_')
        correctedoptions.append('%02d' % (int(valuedict['hour'])) )
        correctedoptions.append(':')
        correctedoptions.append('%02d' % (int(valuedict['minute'])) )
        correctedoptions.append(':')
        correctedoptions.append('%02d' % (int(valuedict['second'])) )
        
        return ''.join([bla for bla in correctedoptions])
    
    def stringToValue(self, thestring):
        expr = self.getValueExpression()
        valuedict = re.match(expr, thestring).groupdict() 
        thetime = datetime.datetime(int(valuedict['year']), int(valuedict['month']), int(valuedict['day']), int(valuedict['hour']), int(valuedict['minute']), int(valuedict['second']))
        return thetime #(datetime.datetime.now()-thetime).seconds
    
    def optionsToValue(self, options):
        expr = self.fullexpression
        valuedict = re.match(expr, options).groupdict() 
        thetime = datetime.datetime(int(valuedict['year']), int(valuedict['month']), int(valuedict['day']), int(valuedict['hour']), int(valuedict['minute']), int(valuedict['second']))
        return thetime #(datetime.datetime.now()-thetime).seconds
    
    def valueToString(self, thetime):
        correctedoptions = []
        
        correctedoptions.append('%02d' % (thetime.day) )
        correctedoptions.append('.')
        correctedoptions.append('%02d' % (thetime.month) )
        correctedoptions.append('.')
        correctedoptions.append('%04d' % (thetime.year) )
        correctedoptions.append('_')
        correctedoptions.append('%02d' % (thetime.hour) )
        correctedoptions.append(':')
        correctedoptions.append('%02d' % (thetime.minute) )
        correctedoptions.append(':')
        correctedoptions.append('%02d' % (thetime.second) )
        
        return ''.join([bla for bla in correctedoptions])
    
    def getStdVal(self):
        return datetime.datetime.now() - datetime.timedelta(minutes = self.stdseconddiff)
   
    def toHtml(self, options):
        try:
            defaultdate = self.optionsToValue(options)
        except:
            defaultdate = self.getStdVal()
        
        return render_to_string(self.template, {'name':self.name, 'defaultdate':self.valueToString(defaultdate), 'userfriendlyname':self.userfriendlyname})
