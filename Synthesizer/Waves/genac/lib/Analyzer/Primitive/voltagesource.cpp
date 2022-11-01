#include "voltagesource.h"
#include "analyzer.h"
#include "dbg.h"

void VoltageSource::stamp(Analyzer* a)
{
    ++a->eqnCounter();
    uint counter = a->eqnCounter();
    int len = nodes().size();
    uint idx[len];
    bool ok = true;
    for(int i = 0; i<len; ++i)
    {
        idx[i] = parent()->index(nodes().at(i),&ok);
        if(!ok)
        {
            qDebug() << "Error: Unknown Node!";
            return;
        }
    }

    if(idx[0] != 0)
    {
        a->C(counter,idx[0]) += 1;
        a->B(idx[0],counter) += 1;
    }
    if(idx[1] != 0)
    {
        a->C(counter,idx[1]) += -1;
        a->B(idx[1],counter) += -1;
    }

    a->J(counter) =
     GiNaC::realsymbol((QString("I(%1)").arg(name())).toStdString());

    double val = value().toDouble(&ok);
    if(ok)
    {
       a->E(counter) += val;
    }
    else
    {
        GiNaC::realsymbol sym;
        a->makeSymbol(sym,value());
        a->E(counter) += sym;
    }

}
