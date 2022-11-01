#ifndef JSHELPER_H
#define JSHELPER_H

#include <QObject>

class JsHelper : public QObject
{
    Q_OBJECT
public:
    JsHelper();
    void displayEquation(QString equation);

signals:
    void showEquation(QString);
};

#endif // JSHELPER_H
