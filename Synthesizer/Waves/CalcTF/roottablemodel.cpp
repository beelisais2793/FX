#include "roottablemodel.h"


RootTableModel::RootTableModel()
{
    insertRow(rowCount());
}

int RootTableModel::rowCount(const QModelIndex &parent) const
{
    return m_rootList.count();
}

int RootTableModel::columnCount(const QModelIndex &parent) const
{
    return RootTableModel::COLUMNCOUNT;
}

QVariant RootTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    Root * item = getItem(index);

    return item->data(index.column());
}

bool RootTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    Root * item = getItem(index);
    bool result = item->setData(index.column(), value);

    if (result)
        emit dataChanged(index, index);

    return result;
}

QVariant RootTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return Root::header(section);

    return QVariant();
}

bool RootTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    bool success = true;

    beginInsertRows(parent, row, row + count - 1);
    for(int i = row; i < row + count; i++){
        Root *r = new Root();
        m_rootList.insert(i,r);
        //qDebug()<<"row inserted row:"<<i;
    }
    endInsertRows();

    return success;
}

bool RootTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    bool success = true;

    beginRemoveRows(parent, row, row + count - 1);
    for(int i = row; i < row + count; i++){
        delete m_rootList[row];
        m_rootList.removeAt(row);
    }
    endRemoveRows();

    return success;
}

Qt::ItemFlags RootTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

Root *RootTableModel::getRoot(const int row)
{
    if (row >=0 && row < rowCount())
        return m_rootList[row];

    return nullptr;
}

QList<Root*> RootTableModel::getRootList()
{
    return m_rootList;
}

void RootTableModel::newRoot()
{
    insertRows(rowCount(),1);
}

void RootTableModel::removeRoot(const int row)
{    
    removeRows(row,1);
}

Root * RootTableModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        Root *item = m_rootList[index.row()];
        if (item)
            return item;
    }
    return new Root();
}

//int RootTableModel::COLUMNCOUNT = 2;


