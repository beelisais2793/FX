#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <QFrame>
#include <QApplication>
#include <QDesktopWidget>
#include <QPoint>
#include <QTimer>

namespace Ui {
class Annotation;
}

class Annotation : public QFrame
{
    Q_OBJECT

public:
    explicit Annotation(QWidget *parent = 0);
    void reloadSplash(QImage &new_splash);
    ~Annotation();
private:
    void mousePressEvent(QMouseEvent *ev);
    void keyPressEvent(QKeyEvent *ev);
    Ui::Annotation *ui;
};

#endif // ANNOTATION_H
