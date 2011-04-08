'''
Created on Jul 13, 2010
The attribute translators apply higher level processing on attribute values

For example the FileExtensionTranslator translates a column, which is hopefully text,
by searching for a file extension (like .raw) and returning a hash value of that extension.

That translates a filename to a number
@author: kblaszcz
'''

import string

class AttributeTranslatorInterface(object):
    def __init__(self):
        pass
    
    def translate(self, modelinstance, attrname):
        raise Exception("implementation of translate() not found")
 
#Attaches a prefix to a string   
class AddPrefixTranslator(AttributeTranslatorInterface):
    
    def __init__(self, prefix):
        AttributeTranslatorInterface.__init__(self)
        self.prefix = prefix
        
    def translate(self, modelinstance, attrname):
        try:
            ext = modelinstance.__dict__[attrname]
        except KeyError:
            raise Exception("attribute doesn't exist")
        
        if not ((type(ext).__name__ == 'str') or (type(ext).__name__ == 'unicode')):
            raise Exception("string or unicode expected")

        return (self.prefix + ext)
# /castor/cern.ch/user => user, /castor/cern.ch/user/m => m   
class TopDirNameTranslator(AttributeTranslatorInterface):
    
    def __init__(self):
        AttributeTranslatorInterface.__init__(self)
        
    def translate(self, modelinstance, attrname):
        try:
            ext = modelinstance.__dict__[attrname]
        except KeyError:
            raise Exception("attribute doesn't exist")
        
        if not ((type(ext).__name__ == 'str') or (type(ext).__name__ == 'unicode')):
            raise Exception("string or unicode expected")

        pos = ext.rfind('/')
        if pos >= 0: ext = ext[pos:]
        return ext
#castor/cern.ch/uer/k/kblaszcz/somefile.root => root => [hash]
#extracts file exteinsion and calclulates hash value between min and max
class FileExtensionTranslator(AttributeTranslatorInterface):

    def __init__(self, min = 0, max = 29):
        AttributeTranslatorInterface.__init__(self)
        if max < min: max, min = min, max
        self.max = max
        self.min = min
            
        
        
    def translate(self, modelinstance, attrname):
        try:
            ext = self.findExtension(modelinstance.__dict__[attrname])
            if(ext == ''): return None
        except KeyError:
            raise Exception("attribute doesn't exist")
        
        if not ((type(ext).__name__ == 'str') or (type(ext).__name__ == 'unicode')):
            raise Exception("string or unicode expected")
        ret = int(self.calcExtHash(ext)%(self.max-self.min))+self.min
        return ret

    def findExtension(self, text):
        dot = string.rfind(text, '.')
        slash = string.rfind(text, '/')
        if dot < slash: return '' #otherwise /castor/cern.ch/user would return an extension ch/user
        if dot < 0 or (len(text)-dot)>16: 
            return ''
        else:
            return text[dot+1:]
        
    def calcExtHash(self, ext):
        hash = 0
        for idx, char in enumerate(ext):
            c = ord(char)
            significant = ((c&23)|((c&16)>>1))&15
            hash = hash + significant << (idx*4)
        return hash

#returns just the raw metric value      
class RawLinearTranslator(AttributeTranslatorInterface):
    
    def __init__(self, minsat = 0.0, maxsat = 1.0):
        AttributeTranslatorInterface.__init__(self)
        self.typename = None
        self.isfloat = None
        self.istext = None
        
    def translate(self, modelinstance, attrname):
        try:
            ext = modelinstance.__dict__[attrname]
        except KeyError:
            raise Exception("attribute doesn't exist")
        
        ext.isfloat = False
        ext.istext = False
        if (type(ext).__name__ == 'float'):
            ext.isfloat = True
        elif ((type(ext).__name__ == 'str') or (type(ext).__name__ == 'unicode')):
            ext.istext = True

        return ext
    
#class isFileByExtensionTranslator(AttributeTranslatorInterface):
#
#    def __init__(self, min = 0, max = 29):
#        AttributeTranslatorInterface.__init__(self)
#        if max < min: max, min = min, max
#        self.max = max
#        self.min = min
#
#    def translate(self, modelinstance, attrname):
#        try:
#            ext = self.findExtension(modelinstance.__dict__[attrname])
#            if(ext == ''): 
#                return 0.0 
#        except KeyError:
#            raise Exception("attribute doesn't exist")
#        
#        if not ((type(ext).__name__ == 'str') or (type(ext).__name__ == 'unicode')):
#            raise Exception("string or unicode expected")
#        return 1.0
#
#    def findExtension(self, text):
#        dot = string.rfind(text, '.')
#        if dot < 0 or (len(text)-dot)>16: 
#            return ''
#        else:
#            return text[dot+1:]

##value gets cut to maximum when it exceeds maximum
#class CutToMaxTranslator(AttributeTranslatorInterface):
#    
#    def __init__(self, minsat = 0.0, maxsat = 1.0):
#        AttributeTranslatorInterface.__init__(self)
#        if maxsat < minsat: maxsat ,minsat = minsat, maxsat
#        self.maxsat = max
#        self.minsat = min
#        
#    def translate(self, modelinstance, attrname):
#        try:
#            ext = self.findExtension(modelinstance.__dict__[attrname])
#        except KeyError:
#            raise Exception("attribute doesn't exist")
#        
#        if not ((type(ext).__name__ == 'int') or (type(ext).__name__ == 'long')) or (type(ext).__name__ == 'float'):
#            raise Exception("number expected")
#        
#        if ext >= self.maxsat:
#            return 1.0
#        elif ext <= self.minsat:
#            return 0.0
#        else:
#            return (ext-self.minsat)/(self.maxsat-self.minsat)
    