#ifndef RESISTOR_H
#define RESISTOR_H

#include "primitive.h"
#include "mapper.h"
class Resistor : public Primitive
{
public:
    Resistor(const QString& name = QString(),
              const QStringList& nodes = QStringList(),
              const QString& value = QString())
        :Primitive(name,nodes,value){}

    virtual void stamp(Analyzer *a);
};

#endif // RESISTOR_H
