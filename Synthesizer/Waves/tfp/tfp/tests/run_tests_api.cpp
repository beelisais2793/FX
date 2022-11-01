#include "tfp/tfp.hpp"
#include "tfp/plugin/PluginManager.hpp"
#include "tfp/plugin/Plugin.hpp"
#include "tfp/views/DataTree.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace tfp;

extern "C" {

int tfp_run_tests(int* argc, char** argv)
{
    std::cout << "Running tests from tfp_run_tests() in tfp library\n";
    return RUN_ALL_TESTS();
}

int tfp_run_plugin_tests(int* argc, char** argv, const char* pluginName)
{
    std::cout << "Running tests for plugin \"" << pluginName << "\" from tfp_run_all_tests() in tfp library\n";

    PluginManager* pm = new PluginManager(new DataTree);
    Plugin* plugin = pm->loadPlugin(pluginName);
    if (plugin == NULL)
    {
        std::cout << "Failed to load plugin" << std::endl;
    }

    int result = plugin->runTests(argc, argv);
    pm->unloadPlugin(plugin);
    delete pm;

    return result;
}

int tfp_run_all_tests(int* argc, char** argv)
{
    std::cout << "Running tests from tfp_run_all_tests() in tfp library\n";
    int thisResult = RUN_ALL_TESTS();

    PluginManager* pm = new PluginManager(new DataTree);
    QStringList failedPluginTests;
    QStringList pluginList = pm->listAvailablePlugins();
    for (QStringList::const_iterator it = pluginList.begin(); it != pluginList.end(); ++it)
    {
        Plugin* plugin = pm->loadPlugin(*it);
        if (plugin == NULL)
            continue;
        if (plugin->runTests(argc, argv) != 0)
            failedPluginTests.push_back(plugin->name());
        pm->unloadPlugin(plugin);
    }
    delete pm;

    std::cout << "----------------------------------------------------------" << std::endl;
    if (thisResult != 0 || failedPluginTests.size() != 0)
        std::cout << "Unit tests failed in the following libraries!" << std::endl;
    else
        std::cout << "All unit tests passed!" << std::endl;
    for (QStringList::const_iterator it = failedPluginTests.begin(); it != failedPluginTests.end(); ++it)
        std::cout << "  - plugin \"" << it->toStdString() << "\"" << std::endl;
    if (thisResult != 0)
        std::cout << "  - tfp library" << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;

    if (thisResult != 0 || failedPluginTests.size() != 0)
        return -1;
    return 0;
}

}
