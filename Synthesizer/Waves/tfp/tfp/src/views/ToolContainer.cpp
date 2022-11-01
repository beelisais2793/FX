#include "tfp/views/ToolContainer.hpp"
#include "tfp/plugin/Tool.hpp"
#include "tfp/plugin/PluginManager.hpp"
#include <QToolBar>
#include <QComboBox>

using namespace tfp;

// ----------------------------------------------------------------------------
ToolContainer::ToolContainer(QWidget* parent) :
    QMainWindow(parent),
    pluginManager_(NULL),
    toolBar_(new QToolBar),
    toolsList_(new QComboBox),
    tool_(NULL)
{
    toolBar_->addWidget(toolsList_);
    addToolBar(Qt::BottomToolBarArea, toolBar_);

    connect(toolsList_, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onToolsListIndexChanged(const QString&)));
}

// ----------------------------------------------------------------------------
ToolContainer::~ToolContainer()
{
}

// ----------------------------------------------------------------------------
void ToolContainer::setPluginManager(PluginManager* pluginManager)
{
    pluginManager_ = pluginManager;
}

// ----------------------------------------------------------------------------
void ToolContainer::setSystem(System* system)
{
    system_ = system;
}

// ----------------------------------------------------------------------------
void ToolContainer::populateToolsList(Plugin::Category category)
{
    toolsList_->clear();
    QVector<ToolFactory*> toolsList = pluginManager_->getToolsList();
    for (QVector<ToolFactory*>::const_iterator it = toolsList.begin(); it != toolsList.end(); ++it)
        if ((*it)->category == category)
            toolsList_->addItem((*it)->name);
}

// ----------------------------------------------------------------------------
void ToolContainer::setTool(const char* toolName)
{
    for (int i = 0; i != toolsList_->count(); ++i)
        if (toolsList_->itemText(i) == toolName)
        {
            toolsList_->setCurrentIndex(i);
            break;
        }
}

// ----------------------------------------------------------------------------
void ToolContainer::onToolsListIndexChanged(const QString& text)
{
    Tool* tool = pluginManager_->createTool(text);
    if (tool == NULL)
        return;

    tool->setSystem(system_);
    setCentralWidget(tool);
}
