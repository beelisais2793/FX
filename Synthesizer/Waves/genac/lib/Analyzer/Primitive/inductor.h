#ifndef INDUCTOR_H
#define INDUCTOR_H

#include "primitive.h"
#include "mapper.h"
class Inductor : public Primitive
{
public:
    Inductor(const QString& name = QString(),
              const QStringList& nodes = QStringList(),
              const QString& value = QString())
        :Primitive(name,nodes,value){}

    virtual void stamp(Analyzer *a);
};

#endif // INDUCTOR_H
