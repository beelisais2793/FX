#ifndef CCVS_H
#define CCVS_H

#include "primitive.h"
#include "mapper.h"
class CCVS : public Primitive
{
public:
    CCVS(const QString& name = QString(),
              const QStringList& nodes = QStringList(),
              const QString& voltageName = QString(),
              const QString& value = QString())
        :Primitive(name,nodes,value),
         m_voltageName(voltageName){}

    virtual void stamp(Analyzer *a);
private:
    QString m_voltageName;
};

#endif // CCVS_H
