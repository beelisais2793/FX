#ifndef VOLTAGESOURCE_H
#define VOLTAGESOURCE_H

#include "primitive.h"
#include "mapper.h"
class VoltageSource : public Primitive
{
public:
    VoltageSource(const QString& name = QString(),
              const QStringList& nodes = QStringList(),
              const QString& value = QString())
        :Primitive(name,nodes,value){}

    virtual void stamp(Analyzer *a);
};

#endif // VOLTAGESOURCE_H
