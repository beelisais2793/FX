#ifndef VCVS_H
#define VCVS_H

#include "primitive.h"
#include "mapper.h"
class VCVS : public Primitive
{
public:
    VCVS(const QString& name = QString(),
              const QStringList& nodes = QStringList(),
              const QString& value = QString())
        :Primitive(name,nodes,value){}

    virtual void stamp(Analyzer *a);
};

#endif // VCVS_H
