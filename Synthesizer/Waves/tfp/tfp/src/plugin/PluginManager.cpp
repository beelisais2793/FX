#include "tfp/plugin/PluginManager.hpp"
#include "tfp/plugin/Plugin.hpp"
#include "tfp/views/DataTree.hpp"
#include "tfp/listeners/PluginListener.hpp"
#include <QLibrary>
#include <QDir>
#include <gmock/gmock.h>

namespace tfp {

// ----------------------------------------------------------------------------
PluginManager::PluginManager(DataTree* dataTree) :
    dataTree_(dataTree)
{
}

// ----------------------------------------------------------------------------
PluginManager::~PluginManager()
{
    unloadAllPlugins();
}

// ----------------------------------------------------------------------------
QStringList PluginManager::listAvailablePlugins() const
{
    QStringList result;
    QDir pluginsDir("plugins");
    QStringList allFiles = pluginsDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);

    QStringList validExtensions;
    validExtensions += ".dll";
    validExtensions += ".so";
    validExtensions += ".dylib";
    validExtensions += ".dynlib";

    for (QStringList::const_iterator it = allFiles.begin(); it != allFiles.end(); ++it)
        for (QStringList::const_iterator ext = validExtensions.begin(); ext != validExtensions.end(); ++ext)
            if (it->endsWith(*ext, Qt::CaseInsensitive))
            {
                result.push_back(*it);
                break;
            }

    return result;
}

// ----------------------------------------------------------------------------
Plugin* PluginManager::loadPlugin(const QString& name)
{
    /*
     * Because googletest stores all of the unit tests globally, we are forced
     * to clear all unit tests every time a new plugin is loaded, so only the
     * unit tests that are in the plugin are registered.
     */
    CLEAR_ALL_TESTS();

    Reference<Plugin> plugin(new Plugin(name));
    plugin->library_->setFileName(QDir::toNativeSeparators("plugins/" + name));
    if (plugin->library_->load() == false)
    {
        std::cout << "Failed to load plugin: " << plugin->library_->errorString().toStdString() << std::endl;
        return NULL;
    }

    /*
     * Load plugin symbols
     */
    if ((plugin->start = (Plugin::start_plugin_func)plugin->library_->resolve("start_plugin")) == NULL)
    {
        std::cout << "Failed to load plugin: " << "symbol start_plugin() not found" << std::endl;
        return NULL;
    }
    if ((plugin->stop = (Plugin::stop_plugin_func)plugin->library_->resolve("stop_plugin")) == NULL)
    {
        std::cout << "Failed to load plugin: " << "symbol start_plugin() not found" << std::endl;
        return NULL;
    }
    if ((plugin->run_tests = (Plugin::run_tests_func)plugin->library_->resolve("run_tests")) == NULL)
    {
        std::cout << "Plugin " << name.toStdString() << " has no unit tests :/" << std::endl;
    }

    /*
     * Try to start the plugin
     */
    if (plugin->start(plugin, dataTree_) == false)
    {
        std::cout << "Failed to load plugin: " << "start_plugin() returned an error" << std::endl;
        return NULL;
    }

    std::cout << "Loaded plugin " << name.toStdString() << std::endl;
    plugins_.push_back(plugin);

    return plugin;
}

// ----------------------------------------------------------------------------
bool PluginManager::loadAllPlugins()
{
    QStringList pluginList = listAvailablePlugins();

    bool success = true;
    for (QStringList::const_iterator it = pluginList.begin(); it != pluginList.end(); ++it)
        success &= (loadPlugin(*it) != NULL);

    return success;
}

// ----------------------------------------------------------------------------
void PluginManager::unloadPlugin(Plugin* plugin)
{
    unloadPlugin(plugins_.indexOf(plugin));
}

// ----------------------------------------------------------------------------
void PluginManager::unloadPlugin(const QString& fileName)
{
    int i;
    for (i = 0; i != plugins_.size(); ++i)
        if (plugins_[i]->name() == fileName)
        {
            unloadPlugin(i);
            return;
        }
}

// ----------------------------------------------------------------------------
void PluginManager::unloadPlugin(int i)
{
    Plugin* plugin = plugins_[i];

    /*
     * All memory that was allocated by the plugin needs to be deallocated
     * right now, before unloading the library.
     *
     * The widget decorator will clean up the tree of widgets that
     * were allocated by the plugin (we notify it by dispatching an event to it)
     *
     * The factory objects were also allocated by the library, which we destroy
     * by clearing the list of factories.
     */
    plugin->dispatcher.dispatch(&PluginListener::onPluginUnloading);
    plugin->toolFactories_.clear();

    if (plugin->stop != NULL)
        plugin->stop(plugin);

    /*
     * Because googletest stores all of the unit tests globally, we are forced
     * to clear all unit tests every time a plugin is unloaded, so only the
     * unit tests that are in the plugin are registered.
     */
    CLEAR_ALL_TESTS();

    std::cout << "Unloading plugin " << plugin->library_->fileName().toStdString() << std::endl;
    plugin->library_->unload();
    delete plugin->library_;

    plugins_.remove(i);
}

// ----------------------------------------------------------------------------
void PluginManager::unloadAllPlugins()
{
    while (plugins_.size() > 0)
        unloadPlugin(0);
}

// ----------------------------------------------------------------------------
QVector<ToolFactory*> PluginManager::getToolsList() const
{
    QVector<ToolFactory*> list;
    for (Plugins::const_iterator it = plugins_.begin(); it != plugins_.end(); ++it)
        list.append(it->get()->getToolsList());
    return list;
}

// ----------------------------------------------------------------------------
Tool* PluginManager::createTool(const QString& name)
{
    Tool* ret = NULL;
    for (Plugins::iterator it = plugins_.begin(); it != plugins_.end(); ++it)
        if ((ret = (*it)->createTool(name)) != NULL)
            break;
    return ret;
}

}
