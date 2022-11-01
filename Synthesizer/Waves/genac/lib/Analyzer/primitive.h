#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "component.h"

class Primitive : public Component
{
public:
    Primitive(const QString& name = QString(),
              const QStringList& nodes = QStringList(),
              const QString& value = QString())
        :Component(name,nodes),m_value(value){}

    //setters and getters
    const QString& value() const {return m_value;}
    void setValue(const QString& value){m_value = value;}

    //Other functions
    virtual void map(Mapper* parent);
protected:
    QString m_value;
};

#endif // PRIMITIVE_H
