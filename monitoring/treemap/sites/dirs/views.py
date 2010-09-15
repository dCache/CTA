# Create your views here.
from django.conf import settings
from django.conf.urls.defaults import *
from django.core.cache import cache
from django.core.urlresolvers import reverse
from django.db.models.query import QuerySet
from django.http import Http404, HttpResponse, HttpResponseRedirect
from django.shortcuts import render_to_response, render_to_response, \
    get_list_or_404, get_object_or_404
from django.template import Context, loader, resolve_variable
from django.template.loader import render_to_string
from django.utils.hashcompat import md5_constructor
from django.utils.http import urlquote
from django.views.decorators.cache import cache_page
from sites.dirs.models import *
from sites.tools.GroupIdService import resolveGroupId
from sites.treemap.defaultproperties.SquaredViewProperties import *
from sites.treemap.drawing.TreeDesigner import SquaredTreemapDesigner
from sites.treemap.drawing.TreemapDrawers import SquaredTreemapDrawer
from sites.treemap.drawing.metricslinking.MetricsLinker import MetricsLinker
from sites.treemap.drawing.metricslinking.TreeNodeDimensions import *
from sites.treemap.objecttree.Annex import Annex
from sites.treemap.objecttree.ObjectTree import ObjectTree
from sites.treemap.objecttree.TreeBuilder import TreeBuilder
from sites.treemap.objecttree.TreeNode import TreeNode
from sites.treemap.objecttree.TreeRules import LevelRules
from sites.treemap.viewtree.TreeCalculators import SquaredTreemapCalculator
import datetime
import profile
from django import forms
from django.db import models
from django.db.models.base import ModelBase
from sites.tools.ObjectCreator import createObject
from sites.tools.ModelsInspection import *
from django.shortcuts import redirect
import random
import sites.dirs.Presets

def redirectOldLink(request, theid):
    return HttpResponse("Please use the new link: <a href = \"" + settings.PUBLIC_APACHE_URL + "/treemaps/\"> here </a>")
#    return treeView(request, 'Dirs', theid)

def redirectHome(request):
    return redirect(to = settings.PUBLIC_APACHE_URL + '/treemaps/Dirs_3')

def treeView(request, rootmodel, theid, refresh_cache = False):  
    
    time = datetime.datetime.now()
    try:
        #Directory you want to show its content
        root = None
        command = "root = "+ rootmodel.__str__()+ ".objects.get(pk="+theid.__str__()+")" 
        exec(command)
        
    except Dirs.DoesNotExist:
        raise Http404
        return render_to_response("Error") 
    
    imagewidth = 800.0
    imageheight = 600.0
    nbdefinedlevels = getDefaultNumberOfLevels()

    serverdict = settings.LOCAL_APACHE_DICT
    treemapdir = settings.REL_TREEMAP_DICT 
    
    #load levelRules from cookie, if cookie doesn't exist, load defaults
    lr = getCookieRules(request, nbdefinedlevels)
        
    cache_key = calcCacheKey(parentpk = theid, parentmodel = rootmodel, lr = lr)
    cache_expire = settings.CACHE_MIDDLEWARE_SECONDS
    value = cache.get(cache_key)
    #if already in cache
    #if already in cache
    cache_hit = False
    if (value is not None): cache_hit = True
    if cache_hit and not refresh_cache:
        return HttpResponse(value)
    
    start = datetime.datetime.now()
    print 'start generating object tree for ' + root.__str__()
    tb = TreeBuilder(lr)
    
    otree = tb.generateObjectTree(rootobject = root) 
    print 'time until now was: ' + (datetime.datetime.now() - start ).__str__()
    
    start = datetime.datetime.now()
    print 'start calculating rectangle sizes'
        
    props = BasicViewTreeProps(width = imagewidth, height = imageheight)    
    tc = SquaredTreemapCalculator(otree = otree, basic_properties = props)

    tree = tc.calculate()
    print 'time until now was: ' + (datetime.datetime.now() - start ).__str__()
    
    start = datetime.datetime.now()
    print "linking metrics to graphical properties"
    mlinker = MetricsLinker()
    mlinker.addPropertyLink('Annex', 'strokecolor', ConstantDimension(-1))
    mlinker.addPropertyLink('Annex', 'inbordersize', ConstantDimension(2))
    mlinker.addPropertyLink('Annex', 'htmlinfotext', AnnexHtmlInfoDimension())
    mlinker.addPropertyLink('Annex', 'fillcolor', ConstantDimension(-2))
    mlinker.addPropertyLink('Annex', 'radiallight.opacity', ConstantDimension(0.0))
    
    mlinker.addPropertyLink('Dirs', 'fillcolor', LevelDimension())
    mlinker.addPropertyLink('Dirs', 'htmlinfotext', DirHtmlInfoDimension())
    mlinker.addPropertyLink('CnsFileMetadata', 'fillcolor', LevelDimension())
    mlinker.addPropertyLink('Dirs', 'headertext', RawColumnDimension('name', DirNameTransformator('/')))
    mlinker.addPropertyLink('CnsFileMetadata', 'headertext', RawColumnDimension('name'))
    mlinker.addPropertyLink('Dirs', 'htmlinfotext', DirHtmlInfoDimension())
    mlinker.addPropertyLink('CnsFileMetadata', 'htmlinfotext', FileHtmlInfoDimension())
    
    mlinker.addPropertyLink('CnsFileMetadata', 'headertext.isbold', ConstantDimension(False))
    print 'time until now was: ' + (datetime.datetime.now() - start ).__str__()
    
    start = datetime.datetime.now()
    print "designing tree"
    designer = SquaredTreemapDesigner( vtree = tree, metricslinkage = mlinker)
#    profile.runctx('designer.designTreemap()', globals(), {'designer':designer})
    designer.designTreemap()
    print 'time until now was: ' + (datetime.datetime.now() - start ).__str__()
    
    start = datetime.datetime.now()
    print "drawing something"
    drawer = SquaredTreemapDrawer(tree)
    filenm = root.getIdReplacement() + lr.getUniqueLevelRulesId() + "treemap.png"
    print filenm
    drawer.drawTreemap(serverdict + treemapdir + "/" + filenm)
    print 'time until now was: ' + (datetime.datetime.now() - start ).__str__()
    #------------------------------------------------------------
    start = datetime.datetime.now()
    response = respond (request = request, vtree = tree, tooltipfontsize = 12, imagewidth = imagewidth, imageheight = imageheight,\
    filenm = filenm, lrules = lr, cache_key = cache_key, cache_expire = cache_expire, time = time, rootsuffix = root.getIdReplacement(), nblevels = nbdefinedlevels)
    
    del tree
    del otree
    print 'time until now was: ' + (datetime.datetime.now() - start ).__str__()
    
    return response

#@cache_page(60 *60 * 24 * 3) #cache for 3 days
def groupView(request, parentpk, depth, model, refresh_cache = False):    
    depth = int(depth)
    time = datetime.datetime.now()
    try:
        #Directory you want to show its content
        root = None
        command = 'root = ' + model + '.objects.get(pk=' + parentpk.__str__() + ')'
        exec(command)
    except:
        raise Http404
    
    imagewidth = 800.0
    imageheight = 600.0
    nbdefinedlevels = getDefaultNumberOfLevels()

    serverdict = settings.LOCAL_APACHE_DICT
    treemapdir = settings.REL_TREEMAP_DICT
    
    cookielr = getCookieRules(request, nbdefinedlevels)
        
    cache_key = calcCacheKey(parentpk = parentpk, parentmodel = "Annex", depth = depth, lr = cookielr)
    cache_expire = settings.CACHE_MIDDLEWARE_SECONDS
    value = cache.get(cache_key)
    #if already in cache
    cache_hit = False
    if (value is not None): cache_hit = True
    if cache_hit and not refresh_cache:
        return HttpResponse(value)
    
    #define LevelRules
    lr = LevelRules()
    
    #define only the first 2 levels because we only need the level 1 to see if there is an Annex in full size
    for i in range(2):
        lr.appendRuleObject(cookielr.getRuleObject(i))
    
    
    start = datetime.datetime.now()
    print 'start generating first object tree ' + root.__str__()
    tb = TreeBuilder(lr)

    otree = tb.generateObjectTree(rootobject = root)    
    print 'time until now was: ' + (datetime.datetime.now() - start ).__str__()
    
    #---------------------------------------------
    otree.traveseToRoot()
    anxnode = otree.getRootAnnex()
    anx = anxnode.getObject()
    
    #if there is an Annex then display it, otherwise you can just display the directory without it
    if anxnode is not None:
        for i in range(depth):
            print 'start generating object subtree ' + anx.__str__()
            otree = tb.generateObjectTree(anx)
            
            newanxnode = otree.getRootAnnex()
            
            if newanxnode is not None: 
                anx = anx.getCopyWithIncDepth(newexcluded = newanxnode.getObject().getExcludedNodes(), evaluation = newanxnode.getEvalValue())    
            else:
                raise Http404 
        
        lr = LevelRules()        
        for i in range(nbdefinedlevels):
            lr.appendRuleObject(cookielr.getRuleObject(i))  
        
        otree = tb.generateObjectTree(anx) 
            
    else: #no Annex display needed
        
        lr = LevelRules()
        for i in range(nbdefinedlevels):
            lr.appendRuleObject(cookielr.getRuleObject(i))  
            
            otree = tb.generateObjectTree(rootobject = root)     
        
     
    start = datetime.datetime.now()
    print 'start calculating rectangle sizes'
        
    props = BasicViewTreeProps(width = imagewidth, height = imageheight)    
    tc = SquaredTreemapCalculator(otree = otree, basic_properties = props)

    tree = tc.calculate()
        
    print 'time until now was: ' + (datetime.datetime.now() - start ).__str__()

    start = datetime.datetime.now()
    print "linking metrics to graphical properties"
    mlinker = MetricsLinker()
    mlinker.addPropertyLink('Annex', 'strokecolor', ConstantDimension(-1))
    mlinker.addPropertyLink('Annex', 'inbordersize', ConstantDimension(2))
    mlinker.addPropertyLink('Annex', 'htmlinfotext', AnnexHtmlInfoDimension())
    mlinker.addPropertyLink('Annex', 'fillcolor', ConstantDimension(-2))
    mlinker.addPropertyLink('Annex', 'radiallight.opacity', ConstantDimension(0.0))
    
    mlinker.addPropertyLink('Dirs', 'fillcolor', LevelDimension())
    mlinker.addPropertyLink('Dirs', 'htmlinfotext', DirHtmlInfoDimension())
    mlinker.addPropertyLink('CnsFileMetadata', 'fillcolor', LevelDimension())
    mlinker.addPropertyLink('Dirs', 'headertext', RawColumnDimension('name', DirNameTransformator('/')))
    mlinker.addPropertyLink('CnsFileMetadata', 'headertext', RawColumnDimension('name'))
    mlinker.addPropertyLink('Dirs', 'htmlinfotext', DirHtmlInfoDimension())
    mlinker.addPropertyLink('CnsFileMetadata', 'htmlinfotext', FileHtmlInfoDimension())
    
    mlinker.addPropertyLink('CnsFileMetadata', 'headertext.isbold', ConstantDimension(False))
    print 'time until now was: ' + (datetime.datetime.now() - start ).__str__()
    
    start = datetime.datetime.now()
    print "designing tree"
    designer = SquaredTreemapDesigner( vtree = tree, metricslinkage = mlinker)
    designer.designTreemap()
    print 'time until now was: ' + (datetime.datetime.now() - start ).__str__()
    
    start = datetime.datetime.now()
    print "drawing something"
    drawer = SquaredTreemapDrawer(tree)
    filenm = "Annex" + root.getIdReplacement() + lr.getUniqueLevelRulesId() + "treemap.png"
    print filenm
    drawer.drawTreemap(serverdict + treemapdir + "/" + filenm)
    print 'time until now was: ' + (datetime.datetime.now() - start ).__str__()
    #------------------------------------------------------------
    start = datetime.datetime.now()
    response = respond (request = request, vtree = tree, tooltipfontsize = 12, imagewidth = imagewidth, imageheight = imageheight,\
    filenm = filenm, lrules = lr, cache_key = cache_key, cache_expire = cache_expire, time = time, rootsuffix = root.pk, nblevels = nbdefinedlevels )
    
    del tree
    del otree
    print 'time until now was: ' + (datetime.datetime.now() - start ).__str__()
    return response

def changeMetrics(request, theid):
    nblevels = getDefaultNumberOfLevels()
    
    if request.method == 'POST':
        createCookieIfMissing(request, nblevels)
            
        posted = {}
        posted = request.POST
        
        if not PostedDataCheckSuccessful(posted, nblevels):
            return HttpResponse("Posted data is not correct")
        
        model = posted['model']
        level = int(posted['level'])
        metric = posted['metric']
        childrenmethodname = posted['childrenmethod']
        
        cookielr = getCookieRules(request, nblevels)
        
        modulename = getModelsModuleName(model)
            
        parentmethodname = 'getDirParent' #for now
        if(level == -1):
            for i in range(nblevels):
                rule = cookielr.getRuleObject(i)
                rule.createOrUpdate(model, childrenmethodname, parentmethodname, metric, None)
        else:
            cookielr.getRuleObject(level).createOrUpdate(model, childrenmethodname, parentmethodname, metric, None)
                    
        updateUserCookie(request, cookielr, level, model, metric, childrenmethodname)
        
    else:
        raise Http404
        
    request.session.set_test_cookie()
    return redirect(to = '..', args = {'request':request, 'theid':theid, 'refresh_cache': True})

def preset(request, theid):
    nblevels = getDefaultNumberOfLevels()
    
    if request.method == 'POST':
        createCookieIfMissing(request, nblevels)
            
        posted = {}
        posted = request.POST
        
        try:
            presetname = posted['preset']
        except KeyError:
            return redirect(to = '..', args = {'request':request, 'theid':theid})
        
        flatview = False
        try:
            posted['flat_view']
            flatview = True
        except KeyError:
            pass
        
        if presetname not in sites.dirs.Presets.getPresetNames():
            return redirect(to = '..', args = {'request':request, 'theid':theid})
#            return HttpResponse("Posted data is not correct")
        
        lr = sites.dirs.Presets.getPreset(presetname)
        
        if flatview:
            newlr = LevelRules()
            for count, rule in enumerate(lr.getRules()):
                newlr.appendRuleObject(rule)
                if(count == 1): break
            
            for i in range(2, lr.countDefinedLevels()):
                newlr.addRules('Annex', 'getItems', 'getAnnexParent', 'evaluation', i)
                
            lr = newlr
            
        request.session['defaultpreset'] = {'flat': flatview, 'presetname': presetname}
        request.session['levelrules'] = lr
        
    else:
        raise Http404
        
    request.session.set_test_cookie()
    return redirect(to = '..', args = {'request':request, 'theid':theid, 'refresh_cache': True})

def respond(request, vtree, tooltipfontsize, imagewidth, imageheight, filenm, lrules, cache_key, cache_expire, time, rootsuffix, nblevels, metric = ''):
    print "preparing response"
    
    apacheserver = settings.PUBLIC_APACHE_URL
    serverdict = settings.LOCAL_APACHE_DICT
    treemapdir = settings.REL_TREEMAP_DICT
    icondir = settings.REL_ICON_DICT
    
    parentid = vtree.getRoot().getProperty('treenode').getNakedParent().pk
    print "PARENTID ", parentid
    parentidstr = vtree.getRoot().getProperty('treenode').getNakedParent().getIdReplacement()
    
    nodes = vtree.getAllNodes()
    mapparams = [None] * len(nodes)
    tooltipshift = [None] * len(nodes)
    
    for (idx, node) in enumerate(nodes):
        x1 = int(round(node.getProperty('x'),0))
        y1 = int(round(node.getProperty('y'),0))
        x2 = int(round(node.getProperty('x') + node.getProperty('width'),0))  
        hsize = node.getProperty('headersize')
        if((not(vtree.nodeHasChildren(node)))):
            hsize = node.getProperty('height')
        y2 = int(round(node.getProperty('y') + hsize,0))
        
        theid = node.getProperty('treenode').getObject().getIdReplacement()
        info = node.getProperty('htmlinfotext')
        
        mapparams[idx] = (x1,y1,x2,y2,theid,info)
        
        textlines = 1
        oldpos, fpos = 0 ,0
        fpos = info.find('<br>', fpos)
        maxtooltipwidth = 0
        
        while fpos != -1:
            textlines = textlines + 1
            
            tooltipwidth = 0.0
            for character in info[oldpos:fpos]:
                if character.islower():
                    tooltipwidth = tooltipwidth + tooltipfontsize * 0.61
                else:
                    tooltipwidth = tooltipwidth + tooltipfontsize * 0.66
            tooltipwidth = int(tooltipwidth)
                    
            if tooltipwidth > maxtooltipwidth: maxtooltipwidth = tooltipwidth
            oldpos = fpos
            fpos = info.find('<br>', fpos + 1)

        if maxtooltipwidth > 0: 
            tooltipwidth =  maxtooltipwidth
        else:
            tooltipwidth = 600
            
        tooltipheight = int(round(textlines * 12 * 1.6))

        itemwidth = int(round(x2-x1))
        itemheight = int(round(y2-y1)) 
        
        shiftx = 20
        shifty = 20
        
        if(x1 + shiftx + tooltipwidth) > imagewidth:
            shiftx = round(imagewidth - (x1 + tooltipwidth))
            
        if(y1 + shifty) > imageheight + 50:
            shifty = 50
#        elif (y1 + shifty) > imageheight and shifty <= 20:
#            shifty = 2*shifty
            
        tooltipshift[idx] = (shiftx, shifty, tooltipwidth, tooltipheight, theid)
        
    rt = vtree.getRoot().getProperty('treenode').getObject()
    parents = []
    
    while True:
        classname = rt.__class__.__name__
        level = 0
        
        parentmethodname =  lrules.getParentMethodNameFor(level, classname)
        pr = rt.__class__.__dict__[parentmethodname](rt)
        if rt is pr: break
        parents.append(pr)

        rt = pr

    
    parents.reverse()
    navlinkparts = []
    for pr in parents:  
        navlinkparts.append( (pr.name, pr.fullname, pr.getIdReplacement()) )
    
    ruleexplanations = []
    i = int(random.random()*nblevels)
    ruleexplanations.append(lrules.describeRuleToUser(i))
    
    generationtime = datetime.datetime.now() - time
    cookierules = getCookieRules(request, nblevels).getRules()
    
    presetnames = sites.dirs.Presets.getPresetNames()
    
    response = render_to_string('dirs/imagemap.html', \
    {'nodes': nodes, 'parentid': parentidstr, 'filename': filenm, 'mapparams': mapparams, 'navilink': navlinkparts, 'imagewidth': int(imagewidth), 'imageheight': int(imageheight),\
     'tooltipfontsize': tooltipfontsize,'tooltipshift': tooltipshift, 'treemapdir': (apacheserver + treemapdir), 'icondir': apacheserver + icondir, \
     'rootsuffix': rootsuffix, "modeldynamics": generateMenuData(nblevels), 'advanceddefault': getCurrentAdvancedSelections(request), \
     'ruleexplanations': ruleexplanations, 'generationtime': generationtime, 'cookierules': cookierules, 'presetnames': presetnames, \
     'presetdefault':getCurrentPresetSelections(request)} , context_instance=None)
    
    totaltime = datetime.datetime.now() - time
#    response = response + '<!-- <p> <blockquote> Execution and render time: ' + totaltime.__str__() + ' </blockquote> </p> -->'
    print "total time: ", totaltime
    
    request.session.set_test_cookie()
    cache.add(cache_key, response, cache_expire)
    return HttpResponse(response)

def PostedDataCheckSuccessful(posted, nblevels):
    availablemodels = getAvailableModels()
    if posted['model'] not in availablemodels: return False
    
    expectedvalues = generateDropdownValues(nblevels, posted['model'])
    
    level = int(posted['level'])
    if level < -1 or level >= nblevels: return False
    
    postedisexpected = False
    for expected in expectedvalues['metrics']:
        if str(posted['metric']) == expected.value:
            postedisexpected = True 
            break    
    if not postedisexpected: return False
    
    postedisexpected = False
    for expected in expectedvalues['childrenmethods']:
        if str(posted['childrenmethod']) == expected.value:
            postedisexpected = True 
            break    
    if not postedisexpected: return False
    
    return True

class DropDownEntry(object):
    def __init__(self, text, value):
        self.text = text
        self.value = value
        
def generateMenuData(nblevels):
    menu = {}
    
    models = []

    modulename = getModelsModuleName("")
    if not modulename in sys.modules.keys():
        try:
            module = __import__( modulename )
        except ImportError, e:
            raise ConfigError( 'Unable to load module: ' + module )
    else:
        module = sys.modules[modulename]
        
    classes = dict(inspect.getmembers( module, inspect.isclass ))
    
    for classname in classes:
        cls = classes[classname]
        if isinstance(cls, ModelBase):
            modelname = cls._base_manager.model._meta.object_name
            models.append(DropDownEntry(cls.getUserFriendlyName(createObject(modulename , classname)), modelname))
                
    for model in models:
        dropdowns = generateDropdownValues(nblevels, model.value)
        menu[model.value] = dropdowns
        
    return menu
        

def generateDropdownValues(nblevels, themodel):
    #generating drop down menu content
    #create level dropdown
    
    leveldropdown = []
    modeldropdown = []
    metricdropdown = []
    childrenmethoddropdown = []
    
    #level menu
    leveldropdown.append(DropDownEntry('all levels', -1))
    for i in range(nblevels):
        leveldropdown.append(DropDownEntry( "level "+i.__str__(), i))
     
    #model menu
    modulename = None
    themodelfound = False
    
    modulename = getModelsModuleName("")
    if not modulename in sys.modules.keys():
        try:
            module = __import__( modulename )
        except ImportError, e:
            raise ConfigError( 'Unable to load module: ' + module )
    else:
        module = sys.modules[modulename]
        
    classes = dict(inspect.getmembers( module, inspect.isclass ))
    
    for classname in classes:
        cls = classes[classname]
        if isinstance(cls, ModelBase):
            modelname = cls._base_manager.model._meta.object_name
            if modelname == themodel: themodelfound = True
            modeldropdown.append(DropDownEntry(cls.getUserFriendlyName(createObject(modulename , classname)), modelname))
    
    if not themodelfound: raise Exception('the given model couldn\'t be found')
    
    #metrics menu
    cf = ColumnFinder(themodel)
    columns = cf.getColumnnames()
    for column in columns:
        ismetric = True
        command = 'if column in ' + themodel +'.nonmetrics: ismetric = False'
        exec(command)
        if ismetric: metricdropdown.append(DropDownEntry(column, column))
    
    #children menu    
    instance = createObject(modulename, themodel)  
    for membername in instance.__class__.__dict__.keys():
        if ((type(instance.__class__.__dict__[membername]).__name__) == 'function'):
            function = instance.__class__.__dict__[membername]
            try:
                print function.__dict__['methodtype']
                if function.__dict__['methodtype'] == 'children':
                    childrenmethoddropdown.append(DropDownEntry(membername, membername))
            except:
                continue
    
    return {'levels':leveldropdown, 'models':modeldropdown, 'metrics':metricdropdown, 'childrenmethods': childrenmethoddropdown}

def generateLevelDynamics(request, nblevels):
    ld = {}
    for i in range(-1, nblevels):
        ld[i] = getDropdownDefaultsByLevel(request, nblevels,i)
    return ld

def getDropdownDefaultsByLevel(request, nblevels, level, themodel = None):
    #generating drop down menu content
    #create level dropdown
    
    levelvalue = level
    modelvalue = ''
    metricvalue = ''
    childrenmethodvalue = ''
    
    #level menu
    if(levelvalue < 0):
        defselection = getAdvancedSelection()
        defselection['level'] = -1
        return defselection
    
    #model menu
    lr = getCookieRules(request, nblevels) 
    levelmodels = lr.getRuleObject(level).getUsedClassNames()
    levelmodels.remove('Annex')
    
    if(themodel is None):  
        if len(levelmodels > 0):
            modelvalue = levelmodels[0]
        else:
            raise Exception('no models available to user') 
    else:
        if themodel in levelmodels:
            modelvalue = themodel
        else:
            raise Exception("no such model: " + themodel) 
    
    #metrics menu
    metricvalue =  lr.getRuleObject(level).getColumnNameFor(modelvalue)
    
    #children menu    
    childrenmethodvalue=  lr.getRuleObject(level).getMethodNameFor(modelvalue)
    
    return {'level':levelvalue, 'model': modelvalue, 'metric': metricvalue, 'childrenmethod': childrenmethodvalue}

def createCookieIfMissing(request, nblevels):
    try:
        request.session['userhascookie']
        request.session['levelrules']
        request.session['advancedselection']
        request.session['defaultpreset']
    except KeyError:
        request.session['userhascookie'] = True
        request.session['levelrules'] = getDefaultRules(nblevels)
        request.session['advancedselection'] = getAdvancedSelection()
        request.session['defaultpreset'] = getDefaultPresets()
        
def updateUserCookie(request, rules, level, model, metric, childrenmethodname):
    request.session['levelrules'] = rules       
    request.session['advancedselection'] = {'level':level, 'model': model, 'metric':metric, 'childrenmethod': childrenmethodname}

def getCurrentAdvancedSelections(request):
    try:
        request.session['userhascookie']
        return request.session['advancedselection']
    except KeyError:
        request.session['userhascookie'] = True
        return getAdvancedSelection()
    
def getCurrentPresetSelections(request):
    try:
        request.session['userhascookie']
        return request.session['defaultpreset']
    except KeyError:
        request.session['userhascookie'] = True
        return getDefaultPresets()
        
def getAdvancedSelection():
    return {'level':-1, 'model': 'Dirs', 'metric':'totalsize', 'childrenmethod':'getFilesAndDirectories'}

def getDefaultPresets():
    return{'flat': False, 'presetname': 'Default'}
        
def getDefaultRules(nblevels):
    lr = sites.dirs.Presets.getPreset("Directory structure")
    return lr

def getCookieRules(request, nbdefinedlevels):
    lr = None
    try:
        lr = request.session['levelrules']
        if not (lr.rulesAreValid() and lr.indexIsValid(nbdefinedlevels-1)):
            lr = getDefaultRules(nbdefinedlevels)
            createCookieIfMissing(request, nbdefinedlevels)
    except KeyError:
        lr = getDefaultRules(nbdefinedlevels)
        createCookieIfMissing(request, nbdefinedlevels)
        
    return lr

def getDefaultModel():
    return 'Dirs'

def getDefaultNumberOfLevels():
    return 8

def calcCacheKey(parentpk, parentmodel, lr, depth = 0):
    key_prefix = settings.CACHE_MIDDLEWARE_KEY_PREFIX
    fragment_name = parentpk.__str__() + '_'+ depth.__str__() + '_'+ ''.join([ord(bla).__str__() for bla in parentmodel.__str__()])
    vary_on = [(2,3,5,7),(11,17,19,23,29)]
    args = md5_constructor(u':'.join([urlquote(resolve_variable("185", parentpk.__str__() + '_'+ fragment_name))]))     
    lrkeypart = lr.getUniqueLevelRulesId()            
    cache_key = 'template.cache.%s.%s.%s.%s' % (key_prefix, fragment_name, args.hexdigest(), lrkeypart)
    return cache_key
