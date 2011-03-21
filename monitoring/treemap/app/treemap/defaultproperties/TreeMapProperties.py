'''
Created on Jul 13, 2010
suggesting parameters to TreeCalculators and TreeDesigner.
These properties are used as standard values, if nothing else is given.
Using these guideline values, TreeCalculators and TreeDesigner set properties for every ViewNode in ViewTree.

All of this result in following Properties of a ViewNode:

set by TreeCalculators:
treenode - original TreeNode object from data tree generated by TreeBuilder
paddingsize - padding to neighbour rectangles, in px
labelheight - height of the rectangle label, in px
width - rectangle width
height - rectangle height
x - rectangle x pos in the image
y - rectangle y pos in the image

set by TreeDesigner:
labeltext - text string to show in the rectangle label
labeltextisbold - defines if the label text should be bold
htmltooltiptext - defines the tooltip text for that node
fillcolor - color the rectangle should be filled with
level - level inside of the data tree (root is 0, ascending)
radiallight {'brightness': b, 'hue':h, 'opacity': o } - defines brightness, hue and opacity of the "light" shining at the rectangle

@author: kblaszcz
'''
#numbers must be floats!
from app.treemap.objecttree.ObjectTree import ObjectTree
from app.treemap.viewtree.ViewTree import ViewTree
import settings


treemap_props = {
'pxwidth': 800.0, #width
'pxheight': 600.0, #height 

'paddingsize': 2.0,
'paddingsizedecrease': 0.5,
'minpaddingsize': 2.0,

'strokesize': 0.0,
'strokesizedecrease': 0.5,
'minstrokesize': 0.0,

'label': True,
'labelfontsize': 12.0, 
'labelheight': 12.0,
'labeltextisbold': True, 

'radiallightbrightness': 0.4,

'objecttree': None, #will be set when available (by TreeBuilder)
'viewtree': None, #will be set when available (by SquaredTreemapCalculator) 

'levelrules': None,#will be set when available (by the view)
'granularity': 1500.0, #minimum rectangle area factor
'maxchildrenworthy': 500.0, #maximum number of children to traverse (except for root)

'icons': True,
'defaulticonfile': settings.LOCAL_APACHE_DICT + settings.REL_SVG_DICT +  '/paperclip.svg'
}

def checkAndPartiallyCorrectTreemapProps(props):
    height = props['pxheight']
    width = props['pxwidth']
    labelheight = props['labelheight']
    paddingsize = props['paddingsize']
    minpaddingsize = props['minpaddingsize']
    strokesize = props['strokesize']
    labelfontsize = props['labelfontsize']
    radiallightbrightness = props['radiallightbrightness']
    labelfontsize = props['labelfontsize']
    
    if labelheight > height:
        props['labelheight'] = height * 0.02
        
    if paddingsize > min(width, height) * 0.5:
        props['paddingsize'] = 0.0
        
    if minpaddingsize <= paddingsize and minpaddingsize >= 0.0:
        props['minpaddingsize'] = minpaddingsize
        
    assert(isinstance(props['objecttree'], ObjectTree) or isinstance(props['objecttree'], None))
        
    if strokesize > min(width, height) * 0.5:
        props['strokesize'] = 0.0

        
    if labelfontsize > labelheight - strokesize:
        props['labelfontsize'] = labelheight - strokesize
        
    if radiallightbrightness > 1.0 or radiallightbrightness < 0.0:
        props['radiallightbrightness'] = 0.4
    
    assert(isinstance(props['viewtree'], ViewTree) or isinstance(props['viewtree'], None))

        


        