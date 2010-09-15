'''
Created on Sep 10, 2010

@author: kblaszcz
'''
from sites.treemap.objecttree.TreeRules import LevelRules
from django.conf import settings
from sites.dirs.views import getDefaultNumberOfLevels
    
def getPreset(presetname):
    try:
        return presetdict[presetname]
    #if not found return some default
    except KeyError:
        lr = LevelRules()
        for i in range(getDefaultNumberOfLevels()):
            lr.addRules('Dirs', 'getFilesAndDirectories', 'getDirParent', 'totalsize', i)
            lr.addRules('CnsFileMetadata', 'getChildren', 'getDirParent', 'filesize', i)
            lr.addRules('Annex', 'getItems', 'getAnnexParent', 'evaluation', i)
        return lr
    
def getPresetNames():
    return presetdict.keys()
    
    
presetdict = {}

#Preset for Directory structure
lr = LevelRules()
for i in range(getDefaultNumberOfLevels()):
    lr.addRules('Dirs', 'getFilesAndDirectories', 'getDirParent', 'totalsize', i)
    lr.addRules('CnsFileMetadata', 'getChildren', 'getDirParent', 'filesize', i)
    lr.addRules('Annex', 'getItems', 'getAnnexParent', 'evaluation', i)
presetdict["Directory structure"] = lr

#Preset for default
lr = LevelRules()
for i in range(getDefaultNumberOfLevels()):
    lr.addRules('Dirs', 'getFilesAndDirectories', 'getDirParent', 'totalsize', i)
    lr.addRules('CnsFileMetadata', 'getChildren', 'getDirParent', 'filesize', i)
    lr.addRules('Annex', 'getItems', 'getAnnexParent', 'evaluation', i)
presetdict["default"] = lr

#Preset for Number of Files
lr = LevelRules()
for i in range(getDefaultNumberOfLevels()):
    lr.addRules('Dirs', 'getDirs', 'getDirParent', 'nbfiles', i)
    lr.addRules('Annex', 'getItems', 'getAnnexParent', 'evaluation', i)
presetdict["Number of files"] = lr

#Preset for Size on Tape
lr = LevelRules()
for i in range(getDefaultNumberOfLevels()):
    lr.addRules('Dirs', 'getFilesAndDirectories', 'getDirParent', 'sizeontape', i)
    lr.addRules('CnsFileMetadata', 'getChildren', 'getDirParent', 'filesize', i)
    lr.addRules('Annex', 'getItems', 'getAnnexParent', 'evaluation', i)
presetdict["Size on tapes"] = lr

#Preset for Data on Tape
lr = LevelRules()
for i in range(getDefaultNumberOfLevels()):
    lr.addRules('Dirs', 'getFilesAndDirectories', 'getDirParent', 'dataontape', i)
    lr.addRules('CnsFileMetadata', 'getChildren', 'getDirParent', 'filesize', i)
    lr.addRules('Annex', 'getItems', 'getAnnexParent', 'evaluation', i)
presetdict["Data on tapes"] = lr

#Preset for Number of tapes
lr = LevelRules()
for i in range(getDefaultNumberOfLevels()):
    lr.addRules('Dirs', 'getDirs', 'getDirParent', 'nbtapes', i)
    lr.addRules('Annex', 'getItems', 'getAnnexParent', 'evaluation', i)
presetdict["Number of tapes"] = lr

#Preset for Number of files on tape
lr = LevelRules()
for i in range(getDefaultNumberOfLevels()):
    lr.addRules('Dirs', 'getFilesAndDirectories', 'getDirParent', 'nbfilesontape', i)
    lr.addRules('CnsFileMetadata', 'getChildren', 'getDirParent', 'filesize', i)
    lr.addRules('Annex', 'getItems', 'getAnnexParent', 'evaluation', i)
presetdict["Number of files on tapes"] = lr

#Preset for Number of File copies on Tape
lr = LevelRules()
for i in range(getDefaultNumberOfLevels()):
    lr.addRules('Dirs', 'getFilesAndDirectories', 'getDirParent', 'nbfilecopiesontape', i)
    lr.addRules('CnsFileMetadata', 'getChildren', 'getDirParent', 'filesize', i)
    lr.addRules('Annex', 'getItems', 'getAnnexParent', 'evaluation', i)
presetdict["Number of file copies on tapes"] = lr

#Preset for Number of directories
lr = LevelRules()
for i in range(getDefaultNumberOfLevels()):
    lr.addRules('Dirs', 'getDirs', 'getDirParent', 'nbsubdirs', i)
    lr.addRules('Annex', 'getItems', 'getAnnexParent', 'evaluation', i)
presetdict["Number of directories"] = lr

#Preset for Time to migrate
lr = LevelRules()
for i in range(getDefaultNumberOfLevels()):
    lr.addRules('Dirs', 'getDirs', 'getDirParent', 'timetomigrate', i)
    lr.addRules('Annex', 'getItems', 'getAnnexParent', 'evaluation', i)
presetdict["Time to migrate"] = lr

#Preset for Time lost in tape marks
lr = LevelRules()
for i in range(getDefaultNumberOfLevels()):
    lr.addRules('Dirs', 'getDirs', 'getDirParent', 'timelostintapemarks', i)
    lr.addRules('Annex', 'getItems', 'getAnnexParent', 'evaluation', i)
presetdict["Time lost in tape marks"] = lr

#Preset for Optimal time to recall
lr = LevelRules()
for i in range(getDefaultNumberOfLevels()):
    lr.addRules('Dirs', 'getDirs', 'getDirParent', 'opttimetorecall', i)
    lr.addRules('Annex', 'getItems', 'getAnnexParent', 'evaluation', i)
presetdict["Optimal time to recall"] = lr



