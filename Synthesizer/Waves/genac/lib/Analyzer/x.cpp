#include "x.h"
#include "subcircuit.h"
#include "circuit.h"
#include "dbg.h"

void X::stamp(Analyzer *a)
{
    for(int i = 0; i<prototype()->numberOfComponents(); ++i)
    {
        prototype()->componentAt(i)->setParent(this);
        prototype()->componentAt(i)->stamp(a);
    }
}

void X::map(Mapper *parent)
{
    bool exists = false;
    unsigned indx;

    setParent(parent);
    Circuit* r = root();
    if(!r)
    {
        qFatal("NULL root detected!\n");
        return;
    }

    indx = r->N();

    for(int i = 0; i<nodes().size(); ++i)
    {
        parent->mapper(nodes().at(i),&indx);
    }

    r->N() = indx;

    //find subcircuit prototype
    Subcircuit* sub = 0;

    Mapper* ptr = this->parent();
    while(ptr)
    {
        if((sub = ptr->findSubcircuit(protoName())))
            break;

        ptr = ptr->getParent();
    }

    Q_ASSERT_X(ptr != 0,"X::map","Requested subcircuit not found!");

        setPrototype(sub);


    Q_ASSERT_X(nodes().size() == prototype()->nodes().size(),
                   "X::map","Node mismatch of subcircuit and X");


    for(int i = 0; i<nodes().size(); ++i)
    {
        indx = this->parent()->index(nodes().at(i),&exists);
           Q_ASSERT_X(exists,"X::map",
                   "Unknown Node,X node cannot be found in its parent map.\n");
            m_map.insert(prototype()->nodes().at(i),indx);
    }

    for(int i = 0; i<prototype()->numberOfComponents(); ++i)
        prototype()->componentAt(i)->map(this);
}

Circuit* X::getRoot()
{
    return root();
}

Mapper* X::getParent()
{
    return parent();
}

Subcircuit* X::findSubcircuit(const QString &name) const
{

    return prototype()->getSubcircuit(name);
}
