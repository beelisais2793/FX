#ifndef Y_H
#define Y_H

#include "primitive.h"
#include "mapper.h"
class Y : public Primitive
{
public:
    Y(const QString& name = QString(),
              const QStringList& nodes = QStringList(),
              const QString& value = QString())
        :Primitive(name,nodes,value){}

    virtual void stamp(Analyzer *a);
};

#endif // Y_H
