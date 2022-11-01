#include "annotation.h"
#include "ui_annotation.h"
#include <QLabel>

Annotation::Annotation(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::Annotation)
{
    ui->setupUi(this);
    QTimer::singleShot(60000, this, SLOT(close()));
    connect(ui->label, SIGNAL(destroyed()), SLOT(close()));
    //старт по центру
    QPoint center = QApplication::desktop()->availableGeometry().center();
    center.setX(center.x() - this->width() / 2);
    center.setY(center.y() - this->height() / 2);
    if(center.x() > 0 || center.y() > 0)
        this->move(center);
    //или в точке 0, 0
    else this->move(QApplication::desktop()->availableGeometry().topLeft());
}

void Annotation::reloadSplash(QImage &new_splash){
    ui->label->clear();
    ui->label->setPixmap(QPixmap::fromImage(new_splash));
}

Annotation::~Annotation(){
    delete ui;
}

void Annotation::keyPressEvent(QKeyEvent *){
    delete ui->label;
}

void Annotation::mousePressEvent(QMouseEvent *){
    delete ui->label;
}
