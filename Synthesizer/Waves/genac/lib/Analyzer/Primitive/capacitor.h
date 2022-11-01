#ifndef CAPACITOR_H
#define CAPACITOR_H

#include "primitive.h"

class Capacitor : public Primitive
{
public:
    Capacitor(const QString& name = QString(),
              const QStringList& nodes = QStringList(),
              const QString& value = QString())
        :Primitive(name,nodes,value){}

    virtual void stamp(Analyzer *a);
};

#endif // CAPACITOR_H
