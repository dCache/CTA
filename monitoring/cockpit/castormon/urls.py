from django.conf.urls.defaults import patterns, include, url

LOGVIEWER_BASE = 'logviewer'

urlpatterns = patterns('',
    # sls overview page
    url(r'^sls$', 'castormon.cockpit.views.home', {'template_name' : 'cockpit/sls.html'}),

    # Cockpit urls
    url(r'^$', 'castormon.cockpit.views.home', {'template_name' : 'cockpit/home.html'}), # home
    url(r'^metric/(?P<metric_name>\w+)$', 'castormon.cockpit.views.display_metric'), # display a specific metric
    url(r'^data/metric/(?P<metric_name>\w+)$', 'castormon.cockpit.views.get_metric_data'), # used by ajax to get data
    url(r'^data/metric/(?P<metric_name>\w+)/(?P<format_type>\w+)$', 'castormon.cockpit.views.get_metric_data'), # used by ajax to get data
    url(r'^data/metric/(?P<metric_name>\w+)/(?P<timestamp_from>\d+)/(?P<format_type>\w+)$', 'castormon.cockpit.views.get_metric_data'), # used by ajax to get data
    url(r'^data/metric/(?P<metric_name>\w+)/(?P<timestamp_from>\d+)/(?P<timestamp_to>\d+)/(?P<format_type>\w+)$', 'castormon.cockpit.views.get_metric_data'), # used by ajax to get data
    url(r'^pushdata$', 'castormon.cockpit.views.pushdata'), # used by the MAE to push data right into the cockpit

    # DLF urls
    url(r'^' + LOGVIEWER_BASE + '$', 'castormon.logviewer.views.home'),
    url(r'^' + LOGVIEWER_BASE + 'url$', 'castormon.logviewer.views.url_dispatcher'),
    url(r'^' + LOGVIEWER_BASE + 'getdata$', 'castormon.logviewer.views.get_data'),
    url(r'^' + LOGVIEWER_BASE + '/file_id/(?P<file_id>[a-zA-Z0-9_\-]+)$', 'castormon.logviewer.views.display_data'),
    url(r'^' + LOGVIEWER_BASE + '/req_id/(?P<req_id>[a-zA-Z0-9_\-]+)$', 'castormon.logviewer.views.display_data'),
    url(r'^' + LOGVIEWER_BASE + '/tape_id/(?P<tape_id>[a-zA-Z0-9_\-]+)$', 'castormon.logviewer.views.display_data'),

)
