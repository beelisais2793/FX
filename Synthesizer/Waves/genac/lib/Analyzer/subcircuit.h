#ifndef SUBCIRCUIT_H
#define SUBCIRCUIT_H

#include <QtCore/QStringList>
#include "abstractcircuit.h"

class Subcircuit : public AbstractCircuit
{
public:
    Subcircuit(const QString& name,const QStringList& nodes)
        :m_name(name),m_nodes(nodes){}

    const QString& name() const {return m_name;}
    void setName(const QString& name){m_name = name;}

    const QStringList& nodes() const {return m_nodes;}
    void setNodes(const QStringList& nodes){m_nodes = nodes;}

    Component* componentAt(int i) const {return m_components[i];}
    int numberOfComponents() const {return m_components.size();}
private:
    QString m_name;
    QStringList m_nodes;
};

#endif // SUBCIRCUIT_H
