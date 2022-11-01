#pragma once

#include "tfp/config.hpp"
#include "tfp/models/Translation.hpp"
#include "tfp/util/Reference.hpp"
#include "tfp/plugin/Plugin.hpp"
#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

namespace ads {
    class CDockManager;
    class CDockWidget;
}

class QMdiArea;
class QComboBox;

namespace tfp {

class DataTree;
class PluginManager;
class System;

/*!
 * @brief The top-most Qt widget of this application. Handles the menu bar
 * actions and sets up global stuff (like shortcuts, icons, etc.)
 */
class TFP_PUBLIC_API MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //! Constructor
    explicit MainWindow(QWidget* parent=NULL);
    //! Default destructor
    ~MainWindow();

    System* newSystem(const QString& name);
    void deleteSystem(System* system);
    ads::CDockWidget* newDockableTool(Plugin::Category category, const char* toolName);

private slots:
    void onActionQuitTriggered();
    void onActionSettingsTriggered();

private:
    void loadPlugins();

private:
    Ui::MainWindow* ui;
    DataTree* dataTree_;
    ads::CDockManager* dockManager_;
    Reference<PluginManager> pluginManager_;
    Reference<System> activeSystem_;
};

} // namespace tfp
