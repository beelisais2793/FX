#ifndef X_H
#define X_H

#include "component.h"
#include "mapper.h"

class Subcircuit;

class X : public Component,public Mapper
{
public:
    X(const QString& name = QString(),
      const QStringList& nodes = QStringList(),
      const QString& protoName = QString(),
      Subcircuit* prototype = 0)
        :Component(name,nodes),m_protoName(protoName),m_prototype(prototype){}

    //setters and getters
    const QString& protoName() const {return m_protoName;}
    void setProtoName(const QString& protoName){m_protoName = protoName;}

    Subcircuit* prototype() const {return m_prototype;}
    void setPrototype(Subcircuit* sub){m_prototype = sub;}

    //Other functions
    //From Mapable
    virtual void stamp(Analyzer *a);
    virtual void map(Mapper *parent);

    //From Mapper
    virtual Circuit* getRoot();
    virtual Mapper* getParent();
    virtual Subcircuit* findSubcircuit(const QString &name) const;

private:
    QString m_protoName;
    Subcircuit* m_prototype;
};

#endif // X_H
