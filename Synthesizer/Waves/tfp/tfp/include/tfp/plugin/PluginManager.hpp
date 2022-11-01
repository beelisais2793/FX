#pragma once

#include "tfp/config.hpp"
#include "tfp/util/Reference.hpp"
#include <QVector>

namespace tfp {

class DataTree;
class Plugin;
class Tool;
class ToolFactory;

/*!
 * @brief Manages the loading and unloading of shared libraries.
 */
class PluginManager : public RefCounted
{
public:
    PluginManager(DataTree* dataTree);
    ~PluginManager();

    /*!
     * @brief Scans the plugins/ directory for shared libraries that could be
     * potentially loaded.
     */
    QStringList listAvailablePlugins() const;

    /*!
     * @brief Loads a shared library and calls its plugin_start() function.
     * @note Due to googletest weirdness, all unit tests are cleared before loading
     * and unloading each plugin, only to be populated again by the plugin specific
     * unit tests (if available). This is unfortunately necessary because unit
     * tests are stored globally and there is no way to keep unit tests that exist
     * in plugins apart from unit tests that exist in the main application.
     * @return If loading was successful, the new plugin object is returned.
     * If loading was unsuccessful, NULL is returned.
     */
    Plugin* loadPlugin(const QString& fileName);

    /*!
     * @brief Attempts to load all plugins in the plugins/ directory.
     * @return If any of the plugins fails to load, false is returned. If
     * everything loads successfully, true is returned.
     */
    bool loadAllPlugins();

    /*!
     * @brief Unloads the specified plugin.
     * @note All associated tools need to have been destroyed before unloading.
     */
    void unloadPlugin(Plugin* plugin);

    /*!
     * @brief Unloads the specified plugin. If the plugin does not exist, then
     * nothing happens.
     * @note All associated tools need to have been destroyed before unloading.
     */
    void unloadPlugin(const QString& fileName);

    /*!
     * @brief Unloads all loaded plugins.
     */
    void unloadAllPlugins();

    /*!
     * @brief Get a list of all instantiable tools that are registered.
     */
    QVector<ToolFactory*> getToolsList() const;

    /*!
     * @brief Helper function for instantiating the specified tool. This can
     * also be done over the factories returned by getToolsList().
     */
    Tool* createTool(const QString& name);

private:
    void unloadPlugin(int index);

private:
    typedef QVector< Reference<Plugin> > Plugins;
    Plugins plugins_;
    // Stuff to pass to plugins in start()
    DataTree* dataTree_;
};

}
