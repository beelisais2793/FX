
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    auto tick = new QTimer(this);
    frame = new Graphs();
    frame->show();
    frame->setHidden(true);
    ui->setupUi(this);
    connect(ui->hsSpeed, SIGNAL(valueChanged(int)),
            ui->lSpeed, SLOT(setNum(int)));

    connect(ui->sbAngle, SIGNAL(valueChanged(double)),
            ui->openGLWidget, SLOT(setAngle(double)));
    connect(ui->sbImpulse, SIGNAL(valueChanged(double)),
            ui->openGLWidget, SLOT(setImpulse(double)));
    connect(ui->sbMass, SIGNAL(valueChanged(double)),
            ui->openGLWidget, SLOT(setMass(double)));

    connect(ui->sbLength, SIGNAL(valueChanged(double)),
            ui->openGLWidget, SLOT(setLength(double)));
    connect(ui->sbDCoeff, SIGNAL(valueChanged(double)),
            ui->openGLWidget, SLOT(setDamping(double)));
    connect(ui->sbDelta, SIGNAL(valueChanged(double)),
            ui->openGLWidget, SLOT(setDelta(double)));

    connect(ui->openGLWidget, SIGNAL(diffAngle(double)),
            ui->dtAngle, SLOT(display(double)));
    connect(ui->openGLWidget, SIGNAL(diffEnergy(double)),
            ui->dtEnergy, SLOT(display(double)));
    connect(ui->openGLWidget, SIGNAL(diffFreq(double)),
            ui->dtFreq, SLOT(display(double)));

    connect(ui->acGraph, SIGNAL(triggered(bool)),
            frame, SLOT(setHidden(bool)));

    connect(ui->pbStart, SIGNAL(clicked(bool)),
            ui->openGLWidget, SLOT(toggleRunning(bool)));

    //tick->start(16.6);
    //connect(tick, SIGNAL(timeout()),
    //        ui->openGLWidget, SLOT(run()));


}

MainWindow::~MainWindow(){
    delete frame;
    delete ui;
}

