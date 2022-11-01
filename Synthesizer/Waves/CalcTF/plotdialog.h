#ifndef PLOTDIALOG_H
#define PLOTDIALOG_H

#include "customgraph.h"

#include <QDialog>



namespace Ui {
class PlotDialog;
}

class PlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlotDialog(QWidget *parent = 0);
    ~PlotDialog();

    CustomGraph *getCustomGraph() const;
    void setCustomGraph(CustomGraph *value);

private:
    Ui::PlotDialog *ui;
    CustomGraph *customGraph;
};

#endif // PLOTDIALOG_H
