#ifndef COMPONENT_H
#define COMPONENT_H

#include <QtCore/QStringList>
#include "mapable.h"

class Circuit;

class Component : public Mapable
{
public:
    Component(const QString& name = QString()
              ,const QStringList& nodes = QStringList())
        :m_name(name),m_nodes(nodes),m_parent(0){}

    //Setters and Getters
    const QString& name() const {return m_name;}
    void setName(const QString& name){m_name = name;}

    const QStringList& nodes() const {return m_nodes;}
    void setNodes(const QStringList& nodes) {m_nodes = nodes;}

    Mapper* parent() const {return m_parent;}
    void setParent(Mapper* parent) {m_parent = parent;}

    //Other function
    Circuit* root();

protected:
  QString m_name;
  QStringList m_nodes;
  Mapper* m_parent;
};

#endif // COMPONENT_H
