#ifndef CCCS_H
#define CCCS_H

#include "primitive.h"
#include "mapper.h"

class CCCS : public Primitive
{
public:
    CCCS(const QString& name = QString(),
              const QStringList& nodes = QStringList(),
              const QString& voltageName = QString(),
              const QString& value = QString())
        :Primitive(name, nodes, value),
         m_voltageName(voltageName){}

    virtual void stamp(Analyzer *a);
private:
    QString m_voltageName;
};

#endif // CCCS_H
