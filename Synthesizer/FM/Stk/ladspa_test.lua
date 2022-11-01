require('ladspa')
w = ladspa.world()
print(w:size())
library = w[0]
plugin = library[0]
print(plugin:name())
plugin_ptr = library:get_plugin_ptr(0)
plugin_instance = ladspa.plugin_instance(plugin_ptr,44100)
