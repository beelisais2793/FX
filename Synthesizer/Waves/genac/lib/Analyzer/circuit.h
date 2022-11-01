#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "mapper.h"
#include "abstractcircuit.h"

class Analyzer;

class Circuit : public Mapper, public AbstractCircuit
{
public:
    Circuit()
        :m_N(0),m_M(0){}

    unsigned& N(){return m_N;}
    unsigned& M(){return m_M;}

    virtual Circuit* getRoot() {return this;}
    virtual Mapper* getParent(){return 0;}
    virtual Subcircuit* findSubcircuit(const QString &name) const
    {
        return getSubcircuit(name);
    }


    QString getIndexedNode(unsigned i)
    {
        return m_map.key(i, QString("hidden%1").arg(i));
    }

    void do_map();
    void do_stamp(Analyzer* a);

private:
    unsigned m_N;
    unsigned m_M;
};

#endif // CIRCUIT_H
