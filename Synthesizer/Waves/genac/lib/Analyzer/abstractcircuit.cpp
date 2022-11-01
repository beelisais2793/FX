#include "abstractcircuit.h"
#include "subcircuit.h"
#include "component.h"
#include "dbg.h"
 AbstractCircuit::~AbstractCircuit()
{
    qDeleteAll(m_components);m_components.clear();
    qDeleteAll(m_subcircuits);m_subcircuits.clear();
}

void AbstractCircuit::addSubcircuit(Subcircuit *sub)
{

    for(int i = 0; i<m_subcircuits.size(); ++i)
    {
        if(sub->name() == m_subcircuits.at(i)->name())
        {
            qCritical()<<"Double declaration of" <<" "<<sub->name();
            return;
        }
    }

    m_subcircuits.append(sub);
}


void AbstractCircuit::addComponent(Component* component)
{
    m_components.append(component);
}

Subcircuit* AbstractCircuit::getSubcircuit(const QString& name) const
{
    for(int i = 0; i<m_subcircuits.size(); ++i)
    {
        if(name == m_subcircuits.at(i)->name())
            return m_subcircuits[i];
    }
    return 0;
}


Component* AbstractCircuit::getComponent(const QString& name) const
{
    for(int i = 0; i<m_components.size(); ++i)
    {
        if(name == m_components.at(i)->name())
            return m_components.at(i);
    }

    return 0;
}
