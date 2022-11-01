#include "tfp/ui_MainWindow.h"
#include "tfp/models/System.hpp"
#include "tfp/plugin/Plugin.hpp"
#include "tfp/plugin/PluginManager.hpp"
#include "tfp/plugin/Tool.hpp"
#include "tfp/util/Util.hpp"
#include "tfp/views/DataTree.hpp"
#include "tfp/views/MainWindow.hpp"
#include "tfp/views/SettingsView.hpp"
#include "tfp/views/ToolContainer.hpp"
#include "ads/DockManager.h"
#include "ads/DockWidget.h"
#include "ads/DockAreaWidget.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QMdiArea>
#include <QGroupBox>
#include <QComboBox>
#include <QPainter>
#include <QSplitter>
#include <QDir>
#include <qwt_text.h>

namespace tfp {

class MdiAreaWithBackgroundImage : public QMdiArea
{
public:
    explicit MdiAreaWithBackgroundImage(QWidget* parent = 0) : QMdiArea(parent)
    {
        setBackground(QBrush(QColor(190, 190, 190)));
    }
protected:
    virtual void paintEvent(QPaintEvent* e) OVERRIDE
    {
        QMdiArea::paintEvent(e);

        QPainter painter(viewport());
        QPixmap pixmap = QPixmap(":/background/welcome.png").scaled(size(), Qt::KeepAspectRatio);
        painter.drawPixmap((size().width() - pixmap.width()) / 2, 0, pixmap);
    }
};

// ----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    dataTree_(new DataTree),
    dockManager_(NULL),
    pluginManager_(new PluginManager(dataTree_))
{
    ui->setupUi(this);

    // Window icon and title
    setWindowIcon(QIcon(":/icons/icon.ico"));
    setWindowTitle(qtTrId(ID_APP_TITLE));

    loadPlugins();

    QWidget* leftPane = new QWidget;
    leftPane->setLayout(new QVBoxLayout);
    leftPane->layout()->addWidget(dataTree_);

    activeSystem_ = newSystem("System");

    QMainWindow* visualiserContainer = new QMainWindow;
    dockManager_ = new ads::CDockManager(visualiserContainer);

    QSplitter* splitter = new QSplitter;
    ui->centralWidget->layout()->addWidget(splitter);
    splitter->addWidget(leftPane);
    splitter->addWidget(dockManager_);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    ads::CDockWidget* dpsfg   = newDockableTool(Plugin::GENERATOR, "DPSFG");
    ads::CDockWidget* bode    = newDockableTool(Plugin::VISUALISER, "Bode Plot");
    ads::CDockWidget* step    = newDockableTool(Plugin::VISUALISER, "Step Response");

    dockManager_->addDockWidget(ads::TopDockWidgetArea, dpsfg);
    dockManager_->addDockWidget(ads::RightDockWidgetArea, bode);
    dockManager_->addDockWidget(ads::BottomDockWidgetArea, step);


    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(onActionQuitTriggered()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(onActionSettingsTriggered()));
}

// ----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}

// ----------------------------------------------------------------------------
void MainWindow::loadPlugins()
{
    pluginManager_->loadAllPlugins();

    std::cout << "Available tools:" << std::endl;
    QVector<ToolFactory*> toolsList = pluginManager_->getToolsList();
    for (QVector<ToolFactory*>::const_iterator it = toolsList.begin(); it != toolsList.end(); ++it)
    {
        std::cout << "  + "
            << (*it)->name.toStdString() << " ("
            << (*it)->blurb.toStdString() << ") -- "
            << (*it)->author.toStdString()
            << std::endl;
    }
}

// ----------------------------------------------------------------------------
System* MainWindow::newSystem(const QString& name)
{
    System* system = new System(name);
    dataTree_->addSystem(system);
    return system;
}

// ----------------------------------------------------------------------------
void MainWindow::deleteSystem(System* system)
{
    dataTree_->removeSystem(system);
    delete system;
}

// ----------------------------------------------------------------------------
ads::CDockWidget* MainWindow::newDockableTool(Plugin::Category category, const char* toolName)
{
    ToolContainer* toolContainer = new ToolContainer;
    toolContainer->setPluginManager(pluginManager_);
    toolContainer->setSystem(activeSystem_);
    toolContainer->populateToolsList(category);
    toolContainer->setTool(toolName);

    ads::CDockWidget* dockWidget = new ads::CDockWidget("Test");
    dockWidget->setWidget(toolContainer);

    return dockWidget;
}

// ----------------------------------------------------------------------------
void MainWindow::onActionQuitTriggered()
{
    close();
}

// ----------------------------------------------------------------------------
void MainWindow::onActionSettingsTriggered()
{
    SettingsView* settingsView = new SettingsView;
    settingsView->setAttribute(Qt::WA_DeleteOnClose);
    settingsView->show();
}

} // namespace tfp
