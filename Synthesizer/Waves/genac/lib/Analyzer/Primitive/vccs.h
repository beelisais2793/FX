#ifndef VCCS_H
#define VCCS_H

#include "primitive.h"
#include "mapper.h"
class VCCS : public Primitive
{
public:
    VCCS(const QString& name = QString(),
              const QStringList& nodes = QStringList(),
              const QString& value = QString())
        :Primitive(name,nodes,value){}

    virtual void stamp(Analyzer *a);
};

#endif // VCCS_H
