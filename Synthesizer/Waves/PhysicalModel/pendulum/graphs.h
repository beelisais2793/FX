#ifndef GRAPHS_H
#define GRAPHS_H

#include <QDialog>

namespace Ui {
class Graphs;
}

class Graphs : public QDialog
{
    Q_OBJECT

public:
    explicit Graphs(QWidget *parent = 0);
    ~Graphs();

private:
    Ui::Graphs *ui;
};

#endif // GRAPHS_H
