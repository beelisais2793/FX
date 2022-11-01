#include "plotdialog.h"
#include "ui_plotdialog.h"

PlotDialog::PlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotDialog)
{
    ui->setupUi(this);

}

PlotDialog::~PlotDialog()
{
    delete ui;
}

CustomGraph *PlotDialog::getCustomGraph() const
{
    return customGraph;
}

void PlotDialog::setCustomGraph(CustomGraph *value)
{
    customGraph = value;
    customGraph->setPlot(ui->widget);
    customGraph->plotGraph();
}
