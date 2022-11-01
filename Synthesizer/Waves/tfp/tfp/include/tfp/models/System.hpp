#pragma once

#include "tfp/config.hpp"
#include "tfp/listeners/SystemListener.hpp"
#include "tfp/math/TransferFunction.hpp"
#include "tfp/util/RefCounted.hpp"
#include "tfp/util/ListenerDispatcher.hpp"
#include <QVector>

class QTreeWidgetItem;

namespace tfp {

class TFP_PUBLIC_API System : public RefCounted,
                              public TransferFunction<double>
{
public:
    System(const QString& name);
    ~System();

    void notifyParametersChanged();
    void notifyStructureChanged();
    void interestingFrequencyInterval(double* xStart, double* xEnd) const;
    void interestingTimeInterval(double* xStart, double* xEnd) const;

    const QString& name() const;

    void setDataTree(QTreeWidgetItem* dataTree);
    QTreeWidgetItem* dataTree();

    ListenerDispatcher<SystemListener> dispatcher;

private:
    QTreeWidgetItem* dataTree_;
    QTreeWidgetItem* tfItem_;
    QTreeWidgetItem* factorItem_;
    QTreeWidgetItem* polesItem_;
    QTreeWidgetItem* zerosItem_;
    QTreeWidgetItem* coeffBItem_;
    QTreeWidgetItem* coeffAItem_;
    QString name_;
};

}
