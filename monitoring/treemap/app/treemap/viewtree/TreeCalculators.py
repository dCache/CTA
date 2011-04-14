'''
Created on May 25, 2010
This class contains the slicing algorithm, which is central to TreeMaps.
It receives the ObjectTree (with TreeNodes) and generates a ViewTree (with ViewNodes).
ViewNodes can have properties attached, describing how to view the node.

calculate returns a ViewTree where each ViewNode is created and extended with following properties:

treenode - original TreeNode object from data tree generated by TreeBuilder
paddingsize - padding to neighbour rectangles, in px
labelheight - height of the rectangle label, in px
width - rectangle width
height - rectangle height
x - rectangle x pos in the image
y - rectangle y pos in the image

@author: kblaszcz
'''
from app.treemap.objecttree.Annex import Annex
from app.treemap.objecttree.ObjectTree import ObjectTree
from app.treemap.viewtree.ViewNode import ViewNode
from app.treemap.viewtree.ViewTree import ViewTree
import math

class DefaultTreemapCalculator(object):
    '''
    classdocs
    '''
    def __init__(self, treemap_props):
        '''
        Constructor
        '''
        self.treemap_props = treemap_props
        self.paddingsize = self.treemap_props['paddingsize']
        self.minpaddingsize = self.treemap_props['minpaddingsize']
        self.labelheight = self.treemap_props['labelheight']
            
        try:
            if not (isinstance(self.treemap_props['objecttree'], ObjectTree)): raise Exception("Object is not of type ObjectTree")
        except:
            raise Exception("treemap_props['objecttree'] doesn't exist or is None. You might use TreeBuilder. TreeBuilder creates the tree and updates your treemap_props ")
        self.otree = self.treemap_props['objecttree']
        
    def calculate(self, optimizefortxt = False, ordered = True, bigdifftreshold = 1.5, squareoverflowdecision = False, strip = False):
        width = self.treemap_props['pxwidth']
        height = self.treemap_props['pxheight']
        
        try:
            if not (isinstance(self.treemap_props['objecttree'], ObjectTree)): raise Exception("Object is not of type ObjectTree")
        except:
            raise Exception("treemap_props['objecttree'] doesn't exist or is None. You might use TreeBuilder. TreeBuilder creates the tree and updates your treemap_props ")
        self.otree = self.treemap_props['objecttree']
        
        self.otree.traveseToRoot()
        root = self.otree.getCurrentObject()
        viewtree = ViewTree(treemap_props = self.treemap_props)
        self.treemap_props['viewtree'] = viewtree
        
        vnode = ViewNode()
        vnode.treenode = root
        if self.treemap_props['labels']:
            vnode.labelheight = self.labelheight
            vnode.labelwidth = width
        else:
            vnode.labelheight = 0.0
            vnode.labelwidth = 0.0
            
        vnode.paddingsize = self.paddingsize
        vnode.x = 0.0
        vnode.y = 0.0
        vnode.width = width
        vnode.height = height
        vnode.level = self.otree.getCurrentObject().getDepth()
        if self.otree.getCurrentAnnexChild() is None:
            vnode.hasannex = False
        else:
            vnode.hasannex = True
            
        viewtree.setRoot(vnode)
        viewtree.traverseIntoChild(vnode)
        
        x = self.paddingsize/2.0
        y = self.paddingsize/2.0
        if self.treemap_props['labels']: y = y + self.labelheight
        width = width - self.paddingsize
        height = height - self.paddingsize
        if self.treemap_props['labels']: height = height - self.labelheight
        
        self.calculateRecursion.__dict__['notextcount'] = 0
        self.calculateRecursion.__dict__["ratiocount"] = 0
        self.calculateRecursion.__dict__["ratiosum"] = 0.0
        self.calculateRecursion.__dict__["qualitysum"] = 0.0
        
        self.calculateRecursion(x, y, width ,height , viewtree, self.paddingsize, self.minpaddingsize, self.labelheight, optimizefortxt, ordered, bigdifftreshold, squareoverflowdecision, strip)
        
        print "notextcount: ", self.calculateRecursion.__dict__['notextcount']
        if self.calculateRecursion.__dict__["ratiocount"] > 0: print "AVERAGE RATIO: ", self.calculateRecursion.__dict__["ratiosum"]/self.calculateRecursion.__dict__["ratiocount"]
        return viewtree
        
    #line: The items are sorted graphically in lines of equally tall squares
    #it is like a line of text but with rectangles instead of letters
    def calculateRecursion(self, startx, starty, width ,height, viewtree, paddingsize, minpaddingsize, labelheight, optimizefortxt, ordered, bigdifftreshold, squareoverflowdecision,strip):

        #calculate the area in square pixels
#        print "level ", self.otree.getLevel()
        pixels = float(height * width)
        tnode = self.otree.getCurrentObject()
        
        #if item too tiny, display it in Annex again
        if width <= 1.0 or height <= 1.0: #return
            if not(isinstance(tnode.getObject(), Annex)):
                self.otree.displayCurrentAlsoInAnnex(self.treemap_props)
                return
            else:
                return
#                
#                self.otree.setAnnexChildToConvenientSize(self.treemap_props)
#                self.otree.traverseBack()
#                viewtree.traverseBack()
#                viewtree.deleteChildren()
#                parentvnode = viewtree.getCurrentObject();
#                
#                self.calculateRecursion(parentvnode.x, parentvnode.y, parentvnode.width ,parentvnode.height, viewtree, parentvnode.paddingsize, minpaddingsize, labelheight, optimizefortxt, ordered, bigdifftreshold, squareoverflowdecision)
#                
#                self.otree.traverseIntoChild(oitem)
#                viewtree.traverseIntoChild(vitem)
#                
#                return
        
        #exclude everyhing that would be smaller than 1 pixel
        if pixels < (1.0+paddingsize)*(1.0+paddingsize): return
        startxold = startx
        startyold = starty
        nblines = 0 #counts how many lines are completed before the cleanup code starts
        
        #go deeper only if parent had a label (in case label activated)
        if self.treemap_props['labels']:
            parentcsize = viewtree.getCurrentObject().labelheight
            if parentcsize <= 0.0:
                return
        
        #children at the current position, ordered from the biggest to smallest, the algorithm works only if children[i] <= children[i+1]
        if ordered:
            children = self.otree.getSortedChildren()
        else:
            children = self.otree.getChildren()
        if len(children) <= 0: return
        
        #collect all nodes that can be displayed
        totalchildnodes = [] #for recursive traversal in the ObjectTree
        totalviewnodes = [] #to determine the size of each node
        
        linesum = 0.0 #accumulate line length sum of the rectangles until you go over parents rectangle border
        areasum = 0.0 #sum of the rectangle areas for one line
        percentagesum = 0.0 #needed to calculate how many percent of the parent area are 100% of the line area
        
#        class L(list): pass #removing read only properties from built-in type list to extend dynamically with attributes 
#        line_collection = L() #Nodes collected for current line
#        line_collection.sum = 0 #sum of raw values
        line_collection = []
        
        VERTICAL, HORIZONTAL = range(2)
        if(not strip):
            if width > height:
                direction = VERTICAL
                linelen = height
            else:
                direction = HORIZONTAL
                linelen = width
        else:
            direction = HORIZONTAL
            linelen = width
        
        #variable needed in the for loop. It stores if the previous value already exceeded linelen if slicing was vertically
        includeoverflowold = False
        #sqwidthold to decect big differences between values 
        sqwidthold = 0;
        
        avgratio = 0.0
        avgratioahead = 0.0
        includeoverflow = False
        max_value_in_strip = 0.0
        min_value_in_strip = 1.0
        
        for child in children:
            
            percentage = child.evaluate() # evaluation must be percentage of the parent area
            
            if percentage <= 0: #if child invisible
                continue
            
            area = percentage*pixels #needed area for that child in square pixels
            sqwidth = math.sqrt(area) #in best case you wish to have a square, so calculate the dimension of the ideal square
            
            if squareoverflowdecision:
                if child.evaluate() > max_value_in_strip: max_value_in_strip = child.evaluate()
                if child.evaluate() < min_value_in_strip: min_value_in_strip = child.evaluate()
                #if slicing of the stripe is vertical, don't add the overflow value to the next line_collection, but to the current one
                #in that way there will be higher probability for width > height, which enables to display more horizontal text
                #vertical text is considered less user friendly and is therefore not implemented
                #includetrigger can be true only once per strip because of the "includeoverflowold = includeoverflow" below:
                #in the iteration after includetrigger is true, both variables will be true and therefore includetrigger will be false
                includeoverflow = (direction == VERTICAL) and ((linesum + sqwidth) > linelen)
                includetrigger = (includeoverflowold == False) and (includeoverflow == True) and optimizefortxt
                
                #optimize by detecting big size differences if the incoming values are ordered
                #bigdifftrigger = sqwidthold/sqwidth > 1.5
                #if ordered == False: bigdifftrigger = False 
                bigdifftrigger = False
                #if min and max greater 0
                if min_value_in_strip > 0: bigdifftrigger = math.sqrt(max_value_in_strip)/math.sqrt(min_value_in_strip) > 12.0
                
                stopdecision = (((linesum + sqwidth) > linelen) and not includetrigger) or bigdifftrigger
            else:
                if child.evaluate() > max_value_in_strip: max_value_in_strip = child.evaluate()
                if child.evaluate() < min_value_in_strip: min_value_in_strip = child.evaluate()
                bigdifftrigger = False
                if min_value_in_strip > 0: bigdifftrigger = math.sqrt(max_value_in_strip)/math.sqrt(min_value_in_strip) > 12.0
                #if min and max greater 0
#                if (max_value_in_strip*len(line_collection)) > 0: bigdifftrigger = (linelen * min_value_in_strip*(len(line_collection)))/(max_value_in_strip*(len(line_collection))+min_value_in_strip) < 40.0
                
                avgratioahead = self.calcAvgRatio(line_collection, percentagesum, pixels, linelen, child)
                
                if avgratio >= 1.0 or avgratioahead >= 1.0:
                    stopdecision = True
                    
                    if not optimizefortxt:
                        if (math.fabs(1-avgratio)>math.fabs(1-avgratioahead)):#if next average ratio better
                            stopdecision = False
                    else:
                        if (math.fabs(1-avgratio)>math.fabs(1-avgratioahead)):#if next average ratio better
                            stopdecision = False
                            
                        if (avgratio <= 1.0 and avgratioahead >= 1.0) and (direction == VERTICAL) and not bigdifftrigger:#if text optimazation on, vertical and passing 1.0
                            stopdecision = False
                        
                        if (avgratio <= 1.0 and avgratioahead >= 1.0) and (direction == HORIZONTAL):#if text optimazation on, horizontal and passing 1.0
                            stopdecision = True
                        
                else:
                    stopdecision = False
                
            #if collecting items for current line is completed, because the next addition would go over the border
            #add all items to TreeView except of the current overflow one
            if stopdecision:
                #save the coordinates where the line starts
                xbeginning = startx
                ybeginning = starty
                #they all together have to be squeezed to same height and parent's size
                line_height = areasum/linelen 
                chheight,chwidth = 0.0, 0.0
         
                for ch in line_collection: #calculate the witdth of each child
                    percentage_of_line = ch.evaluate()/percentagesum #normalize percantage values relative to line
                    child_area = percentage_of_line * areasum #area in square pixels which the child will take in that line
                    child_width = child_area/line_height #height and area are known, now you can calculate width of the child
                    
                    if(direction == HORIZONTAL):
                        chwidth = child_width 
                        chheight = line_height
                    else:
                        chheight = child_width
                        chwidth = line_height
                           
                    self.calculateRecursion.__dict__["ratiocount"] = self.calculateRecursion.__dict__["ratiocount"] + 1
                    self.calculateRecursion.__dict__["ratiosum"] = self.calculateRecursion.__dict__["ratiosum"] + (chheight/chwidth)
                    self.calculateRecursion.__dict__["qualitysum"] = self.calculateRecursion.__dict__["qualitysum"] + (min(chheight,chwidth)/max(chheight, chwidth))
                    
                    #store the calculated values
                    vn = ViewNode()
                    if (2*labelheight > (chheight-2.0)) or not self.treemap_props['labels']:
                        vn.labelheight = 0.0
                        vn.labelwidth = 0.0
                    else:
                        vn.labelheight = labelheight
                        vn.labelwidth = chwidth - paddingsize
                        
                    vn.treenode = ch
                    vn.paddingsize = paddingsize
                    vn.x = startx + paddingsize/2.0
                    vn.y = starty + paddingsize/2.0
                    vn.width = chwidth - paddingsize
                    vn.height = chheight - paddingsize
                    vn.level = self.otree.getCurrentObject().getDepth() + 1
                    
                    currently_inscope = self.otree.getCurrentObject()
                    self.otree.traverseIntoChild(ch)
                    if self.otree.getCurrentAnnexChild() is None:
                        vn.hasannex = False
                    else:
                        vn.hasannex = True
                    self.otree.traverseBack()
                    
                    totalchildnodes.append(ch)
                    totalviewnodes.append(vn)
                    viewtree.addChild(vn)
                    
                    #fix start position for next child
                    if(direction == HORIZONTAL):
                        startx = startx + child_width
                    else:
                        starty = starty + child_width
                
                #see what the ratio of the remaining area is
                remaining_ratio = 1.0        
                if(direction == HORIZONTAL):
                    remaining_ratio = (height-(starty-startyold)-chheight)/(width-(xbeginning - startxold))
                else:
                    remaining_ratio = (height-(ybeginning - startyold))/(width-(startx-startxold)-chwidth)
        
                #calculate start coordinates and direction for the next line
                if not strip:
                    if (direction == HORIZONTAL):
                        if remaining_ratio < 1.0:
                            direction = VERTICAL
                            linelen = height - (starty-startyold) -chheight 
                            startx = xbeginning
                            starty = starty + chheight
                        else:
                            starty = starty + chheight
                            startx = xbeginning
                            
                    elif (direction == VERTICAL):
                        if remaining_ratio >= 1.0:
                            direction = HORIZONTAL
                            linelen = width - (startx-startxold) -chwidth
                            starty = ybeginning
                            startx = startx + chwidth
                        else:
                            starty = ybeginning
                            startx = startx + chwidth
                else:
                    linelen = width
                    starty = starty + chheight
                    startx = xbeginning

                
                #take care of the remaining child which belongs to the next line
                linesum = sqwidth
                areasum = area
                percentagesum = percentage
                line_collection = [] #clear the children list for new line
                line_collection.append(child) #add the remaining child
                avgratio = self.calcAvgRatio(line_collection, percentagesum, pixels, linelen)
                max_value_in_strip = child.evaluate()
                min_value_in_strip = max_value_in_strip
                
                nblines = nblines + 1 #counts the number of lines 
                
            else: #number of items not reached yet, accumulate values and append child to collection 
                linesum = linesum + sqwidth 
                areasum = areasum + area
                percentagesum = percentagesum + percentage
                line_collection.append(child)
                avgratio = avgratioahead
                
            #update includeoverflowold for the next iteration
            includeoverflowold = includeoverflow
            #save the square width to have value comparision for the next
            sqwidthold = sqwidth
        
        #cleanup code: In case of unprocessed items in line_collection
        if line_collection:
            #assuming line_collection, areasum, percentagesum have correct values from the previous loop
            xbeginning = startx
            ybeginning = starty
            
            line_height = areasum/linelen
            
            for ch in line_collection:
                percentage_of_line = ch.evaluate()/percentagesum
                child_area = percentage_of_line * areasum
                child_width = child_area/line_height
                
                if(direction == HORIZONTAL):
                    chwidth = child_width
                    chheight = line_height
                else:
                    chheight = child_width
                    chwidth = line_height
                
                if (((chwidth-paddingsize) <= 0) or ((chheight-paddingsize) <= 0)):
                    #self.treemap_props['objecttree'].displayInAnnex(child)
                    continue
                #store the calculated values
                vn = ViewNode()
                if (2.0 * labelheight > (chheight-2.0)) or not self.treemap_props['labels']:
                    vn.labelheight = 0.0
                    vn.labelwidth = 0.0
                else:
                    vn.labelheight = labelheight
                    vn.labelwidth = chwidth-paddingsize
                    
                vn.treenode = ch
                vn.paddingsize = paddingsize
                vn.x = startx+paddingsize/2.0
                vn.y = starty+paddingsize/2.0
                vn.width = chwidth-paddingsize
                vn.height = chheight-paddingsize
                vn.level = self.otree.getCurrentObject().getDepth() + 1
                
                currently_inscope = self.otree.getCurrentObject()
                self.otree.traverseIntoChild(ch)
                if self.otree.getCurrentAnnexChild() is None:
                    vn.hasannex = False
                else:
                    vn.hasannex = True
                    
                self.otree.traverseBack()
                    
                totalchildnodes.append(ch)
                totalviewnodes.append(vn)
                viewtree.addChild(vn)
                    
                #fix start position for next child
                if(direction == HORIZONTAL):
                    startx = startx + child_width
                else:
                    starty = starty + child_width
            
        #now that parent is ready, do recursion
        if paddingsize > minpaddingsize: paddingsize = paddingsize - self.treemap_props['paddingsizedecrease']
        
        for i in range(len(totalchildnodes)):
            #chnname = str(totalchildnodes[i].getObject())
            #print chnname
            self.otree.traverseIntoChild(totalchildnodes[i])
            viewtree.traverseIntoChild(totalviewnodes[i])
            
            #see if it is big enough to have a label and do recursion
            #(startx, starty, width ,height, viewtree, paddingsize, minpaddingsize, labelheight, optimizefortxt, ordered, bigdifftreshold, squareoverflowdecision)
            csize = totalviewnodes[i].labelheight
            if csize <= 0.0:
                self.calculateRecursion.__dict__['notextcount'] = self.calculateRecursion.__dict__['notextcount'] + 1
                self.calculateRecursion(totalviewnodes[i].x +paddingsize/2.0, totalviewnodes[i].y +paddingsize/2.0, totalviewnodes[i].width - paddingsize ,totalviewnodes[i].height-paddingsize, viewtree, paddingsize, minpaddingsize, labelheight, optimizefortxt, ordered, bigdifftreshold, squareoverflowdecision, strip)
            else:
                self.calculateRecursion(totalviewnodes[i].x +paddingsize/2.0, totalviewnodes[i].y + csize + paddingsize/2.0, totalviewnodes[i].width-paddingsize ,totalviewnodes[i].height - csize-paddingsize, viewtree, paddingsize, minpaddingsize, csize, optimizefortxt, ordered, bigdifftreshold, squareoverflowdecision, strip)
                
            self.otree.traverseBack()
            viewtree.traverseBack()
        
    def calcAvgRatio(self, line_collection, thesum, pixels, linelen, child = None):
        if(child == None):
            avgsum = 0.0
            h = thesum*pixels/linelen
            if h != 0:
                width = 0.0
                for item in line_collection:
                    width = (item.evaluate()*pixels)/h
                    if width == 0.0: continue
                    avgsum = avgsum + h/width
            
            if len(line_collection) > 0:
                currentratio = avgsum/len(line_collection)
            else:
                currentratio = 0
            return currentratio
        else:
            avgsum = 0.0
            h = (thesum+child.evaluate())*pixels/linelen
            if h != 0:
                width = 0.0
                for item in line_collection:
                    width = (item.evaluate()*pixels)/h
                    if width == 0.0: continue
                    avgsum = avgsum + h/width
                
                width = (child.evaluate()*pixels)/h
                if width != 0.0:
                    avgsum = avgsum + h/width
                
            return avgsum/(len(line_collection)+1)

    
    
    
    