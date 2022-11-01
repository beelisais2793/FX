#include "graphs.h"
#include "ui_graphs.h"

Graphs::Graphs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Graphs)
{
    ui->setupUi(this);
    this->setLocale(QLocale(QLocale::Russian, QLocale::RussianFederation));

}

Graphs::~Graphs()
{
    delete ui;
}
