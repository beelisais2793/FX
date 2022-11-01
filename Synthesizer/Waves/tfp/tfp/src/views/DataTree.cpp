#include "tfp/views/DataTree.hpp"
#include "tfp/models/System.hpp"

namespace tfp {

// ----------------------------------------------------------------------------
DataTree::DataTree(QWidget* parent) :
    QTreeWidget(parent)
{
    setColumnCount(2);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    //headerItem()->setHidden(true);
}

// ----------------------------------------------------------------------------
void DataTree::addSystem(System* system)
{
    QTreeWidgetItem* item = new QTreeWidgetItem;
    addTopLevelItem(item);
    item->setExpanded(true);
    item->setText(0, system->name());
    system->setDataTree(item);
}

// ----------------------------------------------------------------------------
void DataTree::removeSystem(System* system)
{
    delete takeTopLevelItem(indexOfTopLevelItem(system->dataTree()));
}

}
