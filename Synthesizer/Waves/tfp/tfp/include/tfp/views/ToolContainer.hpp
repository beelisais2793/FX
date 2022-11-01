#pragma once

#include "tfp/config.hpp"
#include "tfp/plugin/Plugin.hpp"
#include <QMainWindow>

class QComboBox;
class QToolBar;

namespace tfp {

class PluginManager;
class System;

/*!
 * @brief Class that manages instantiating/switching between different plugin
 * tools via a dropdown menu in the bottom left.
 */
class ToolContainer : public QMainWindow
{
    Q_OBJECT

public:
    ToolContainer(QWidget* parent=NULL);
    ~ToolContainer();

    /*!
     * @brief The container needs the plugin manager to be able to instantiate
     * tools.
     */
    void setPluginManager(PluginManager* pluginManager);

    /*!
     * @brief The system the tools should modify or visualise.
     */
    void setSystem(System* system);

    /*!
     * @brief Populates the dropdown with tools matching the specified category
     *
     * There can only be one tool container that is able to instantiate
     * "generator" type plugin tools, but there can be many others that can
     * instantiate "visualiser" or "manipulator".
     */
    void populateToolsList(Plugin::Category category);

    void setTool(const char* toolName);

private slots:
    void onToolsListIndexChanged(const QString& text);

private:
    PluginManager* pluginManager_;
    System* system_;
    QToolBar* toolBar_;
    QComboBox* toolsList_;
    QWidget* tool_;
};

}
