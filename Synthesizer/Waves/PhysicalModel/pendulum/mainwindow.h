#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "phyzxmodel.h"
#include "graphs.h"
#include <QMainWindow>
#include <QFrame>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:


signals:
    //void on_hsSpeed_valueChanged(int value);


private:
    Ui::MainWindow *ui;
    Graphs *frame;

};

#endif // MAINWINDOW_H
