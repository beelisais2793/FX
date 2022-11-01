#include "vcvs.h"
#include "analyzer.h"
#include "voltagesource.h"
#include "dbg.h"

void VCVS::stamp(Analyzer* a)
{
    int len = nodes().size();

    if(len != 4)
        qDebug() << "Error: Number of VCVS nodes is inconsistent!";

    uint idx[2];
    bool ok = true;
    for(int i = 2; i < 4; ++i)
    {
        idx[i - 2] = parent()->index(nodes().at(i),&ok);
        if(!ok)
        {
            qDebug() << "Error: Unknown Node!";
            return;
        }
    }

    QStringList nlist;
    nlist << nodes().at(0)<<nodes().at(1);
    VoltageSource* v = new VoltageSource(name(),nlist,"0");
    v->setParent(parent());
    v->stamp(a);
    delete v;

    double val = value().toDouble(&ok);
    if(ok)
    {
        if(idx[0] != 0)
            a->C(a->eqnCounter(),idx[0]) += -1*val;
        if(idx[1] != 0)
            a->C(a->eqnCounter(),idx[1]) += val;
    }
    else
    {
        GiNaC::realsymbol sym;
        a->makeSymbol(sym,value());
        if(idx[0] != 0)
            a->C(a->eqnCounter(),idx[0]) += -1*sym;
        if(idx[1] != 0)
            a->C(a->eqnCounter(),idx[1]) += sym;
    }
}
