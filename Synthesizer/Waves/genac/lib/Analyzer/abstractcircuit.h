#ifndef ABSTRACTCIRCUIT_H
#define ABSTRACTCIRCUIT_H

#include <QtCore/QList>

class Component;
class Subcircuit;

class AbstractCircuit
{
public:
    virtual ~AbstractCircuit();

    void addSubcircuit(Subcircuit* sub);
    void addComponent(Component* component);

    Subcircuit* getSubcircuit(const QString& name) const;
    Component* getComponent(const QString& name) const;

protected:
    AbstractCircuit(){}
    QList<Component*> m_components;
    QList<Subcircuit*> m_subcircuits;

};

#endif // ABSTRACTCIRCUIT_H
