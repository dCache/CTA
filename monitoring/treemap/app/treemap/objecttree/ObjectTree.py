'''
Created on May 10, 2010

The class embeds model instances in TreeNodes and holds them in a tree structure.

@author: kblaszcz
'''
from app.treemap.objecttree.Annex import Annex
from app.treemap.objecttree.TreeNode import TreeNode
import networkx as nx
import warnings

class ObjectTree(object):
    '''
    you insert your model instances, but you receive TreeNode Objects!
    create and traverse the Object tree
    use traveseToRoot() to restore the traversal position
    '''


    def __init__(self):
        self.root = None
        
        self.node_inscope = self.root
        self.children_inscope = []
        self.graph = nx.DiGraph()
        self.depth_inscope = -1
        self.traversal_path = []
        
        self.children_sorted = False
        self.children_cached= False

    def setRoot(self, theobject, parentmethodname, fparam = None, evalattr = None, siblingssum = 1.0):
        self.graph.clear()
        theroot = TreeNode(theobject = theobject, evalattrname = evalattr, parentmethodname = parentmethodname, fpar = fparam, siblings_sum = siblingssum, dpt = 0) 
        self.graph.add_node(theroot, value = 1)
        self.root = theroot
        self.node_inscope = theroot
        self.depth_inscope = 0
        self.children_sorted = False
        self.children_cached= False
        self.traversal_path = []
        
    def getRoot(self):
        return self.root

    def countChildren(self):
        self.updateChildren()    
        return len(self.children_inscope)
    
    def getSortedChildren(self):
        def childrenCompare(child1, child2):
            #self.graph.neighbors doesn't guarantee the same order everytime you call it
            #so if you want to guarantee the same child order (for consistent access by index)
            #you need to avoid equality of evaluation. This is done here by using python's objectid
            if child1.evaluate() > child2.evaluate(): return 1
            elif child1.evaluate() < child2.evaluate(): return -1
            else: 
                if id(child1.getObject()) > (child2.getObject()):
                    return 1
                elif id(child1.getObject()) < (child2.getObject()):
                    return -1
            warnings.warn('you seem to have the same object at least 2 times on the same tree level')
            return 0
                
        if not self.children_sorted:
#            print '--'
#            for node in self.children_inscope:
#                print node.getObject(), node.evaluate()
#            self.children_sorted = True
            self.updateChildren()  
            self.children_inscope.sort(cmp=childrenCompare, key=None, reverse=True)
            self.children_sorted = True
#            print '--'
#            for node in self.children_inscope:
#                print node.getObject(), node.evaluate()
#            self.children_sorted = True
        return self.children_inscope
            
#    count = 0     
            
    def addChild(self, theobject, attrname, parentmethodname, fparam = None):
#        self.count = self.count + 1
#        print self.count
#        if self.count%13 == 0:
#            print "stop here"
        if self.root == None:
            raise Exception( 'Cannot add child if no root specified')
        
        if self.node_inscope is theobject:
            print "error!!!"
            raise Exception( 'Parent cannot contain itself as child')
        
        child = TreeNode(theobject, attrname, parentmethodname, fparam, self.depth_inscope + 1)
        self.graph.add_node(child)
        self.graph.add_edge(self.node_inscope, child)
        if self.children_cached:
            self.children_inscope.append(child)
        self.children_sorted = False
        return child
    
    def addTreeNodeChild(self, theobject):
        if self.root == None:
            raise Exception( 'Cannot add child if no root specified')
        
        if self.node_inscope is theobject:
            print "error!!!"
            raise Exception( 'Parent cannot contain itself as child')
        
        assert(isinstance(theobject, TreeNode))
        
        self.graph.add_node(theobject)
        self.graph.add_edge(self.node_inscope, theobject)
        if self.children_cached:
            self.children_inscope.append(theobject)
        self.children_sorted = False
        return theobject

    def addChildren(self, objects, attrname, parentmethodname, fparam = None):
        if self.root == None:
            raise Exception( 'Cannot add child if no root specified')
    
        if self.node_inscope in objects:
            raise Exception( 'Parent cannot contain itself as child')
    
        for theobject in objects:
            
            child = TreeNode(theobject, attrname, parentmethodname, fparam, self.depth_inscope + 1)
            self.graph.add_node(child)
            self.graph.add_edge(self.node_inscope, child)
            if self.children_cached:
                self.children_inscope.append(child)
                
        self.children_sorted = False
        
    def getChildren(self):
        self.updateChildren()
        return self.children_inscope

    def deleteChildren(self):
        self.graph.remove_nodes_from(self.graph.neighbors(self.node_inscope))
        
        self.children_inscope = []
        
        self.children_sorted = True
        self.children_cached= True

    
    def getCurrentObject(self):
        return self.node_inscope
        

    def traverseIntoChild(self, child):
        if not self.graph.has_node(child) and self.graph.has_edge(self.node_inscope, child):
            raise Exception( 'No child like ' + child.__str__() + ' found')
        if self.root == None:
            raise Exception( 'Cannot go deeper if no root specified')   

        self.traversal_path.append(self.node_inscope)
        self.node_inscope = child
        
        self.children_cached = False
        self.children_sorted = False
        self.depth_inscope = self.depth_inscope + 1

        
    def traverseBack(self):
        if self.depth_inscope <= 0:
            raise Exception( 'root has no parents, cannot traverse back')
        
        #find parent
#        parent_edge = self.graph.in_edges(self.node_inscope)
        if len(self.traversal_path) <= 0:
            raise Exception( 'no parents found, cannot traverse back')
        
        parent = self.traversal_path.pop()
#        parent = parent_edge[0][0] #first edge, first Node
        self.node_inscope = parent
        
        self.children_sorted = False
        self.children_cached = False

        self.depth_inscope = self.depth_inscope - 1
    
    def traveseToRoot(self):
        self.node_inscope = self.root
        self.children_sorted = False
        self.children_cached = False
        self.depth_inscope = 0
        self.traversal_path = []
    
    def updateChildren(self):
        if not self.children_cached:
            self.children_inscope = []
            self.children_inscope = self.graph.neighbors(self.node_inscope)
            if self.children_inscope:
                self.children_sorted = False
            else:
                self.children_inscope = []
                self.children_sorted = True
                
        self.children_cached = True
    
    def getLevel(self):
        return self.depth_inscope
    
    def getRootAnnex(self):
        node_inscope_backup = self.node_inscope
        children_sorted_backup = self.children_sorted
        children_cached_backup = self.children_cached
        depth_inscope_backup = self.depth_inscope
        traversal_path_backup = self.traversal_path
        
        self.traveseToRoot()
        firstlevelitems =self.getChildren()

        #find Annex
        anxnode = None
        for item in firstlevelitems:
            if item.getObject().getClassName() == 'Annex':
                anxnode = item
                break
        
        self.node_inscope = node_inscope_backup
        self.children_sorted = children_sorted_backup 
        self.children_cached = children_cached_backup 
        self.depth_inscope = depth_inscope_backup 
        self.traversal_path = traversal_path_backup 
        
        return anxnode
    
    def displayCurrentInAnnex(self, treemap_props):
        item = self.node_inscope
        self.traverseBack()
        parent = self.node_inscope
        
        anxnode = None
        for sibling in self.children_inscope:
            if sibling.getObject().getClassName() == 'Annex':
                anxnode = sibling
                break
        
        #if Annex container is missing, create one
        if (anxnode is None):
            excludednodes = []
            for sibling in self.children_inscope:
                excludednodes.append(sibling.getObject())
                
            newannex = Annex(treemap_props['levelrules'], self.depth_inscope, parent.getObject(), excludednodes, 0)
            newannex.evaluation = 0.0
            newannex.setCorrectedEvaluation(item.getEvalValue())
            newannex.activateNode(item)
            item.setSiblingsSum(item.getEvalValue())
            
            chclassname = newannex.getClassName()
            chattrname = treemap_props['levelrules'].getAttrNameFor(self.depth_inscope, chclassname)
            chparam = treemap_props['levelrules'].getParamFor(self.depth_inscope, chclassname)
            parentmethodname =  treemap_props['levelrules'].getParentMethodNameFor(self.depth_inscope, chclassname)
            self.addChild(newannex, chattrname, parentmethodname, chparam)
            
        else:
            anxnode.getObject().activateNode(item)
        
        self.traverseIntoChild(item)
        

        