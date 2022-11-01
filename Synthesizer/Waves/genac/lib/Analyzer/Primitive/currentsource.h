#ifndef CURRENTSOURCE_H
#define CURRENTSOURCE_H

#include "primitive.h"
#include "mapper.h"
class CurrentSource : public Primitive
{
public:
    CurrentSource(const QString& name = QString(),
              const QStringList& nodes = QStringList(),
              const QString& value = QString())
        :Primitive(name,nodes,value){}

    virtual void stamp(Analyzer *a);
};

#endif // CURRENTSOURCE_H
