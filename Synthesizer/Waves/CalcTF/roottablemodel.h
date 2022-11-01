#ifndef ROOTTABLEMODEL_H
#define ROOTTABLEMODEL_H

#include "root.h"

#include <QAbstractTableModel>



class RootTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    RootTableModel();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    Root * getRoot(const int row);

    QList<Root *> getRootList();

    void newRoot();
    void removeRoot(const int row);

    const int COLUMNCOUNT = 2;

private:
    Root * getItem(const QModelIndex &index) const;
    QList<Root*> m_rootList;
};

#endif // ROOTTABLEMODEL_H
