#include "inductor.h"
#include "analyzer.h"
#include "dbg.h"

void Inductor::stamp(Analyzer* a)
{
    int len = nodes().size();
    uint idx[len];
    GiNaC::realsymbol s;
    a->makeSymbol(s,"s");
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

    double num = value().toDouble(&ok);
    if(ok)
    {
        GiNaC::numeric val(num);

        if(idx[0] != 0 && idx[1] != 0)
        {
            a->G(idx[0],idx[0]) += 1/(s*val);
            a->G(idx[1],idx[1]) += 1/(s*val);
            a->G(idx[0],idx[1]) -= 1/(s*val);
            a->G(idx[1],idx[0]) -= 1/(s*val);
        }
        else if(idx[0] == 0 && idx[1] == 0)
        {
            a->G(idx[1],idx[1]) += 1/(s*val);
        }
        else if(idx[1] == 0 && idx[0] != 0)
        {
            a->G(idx[0],idx[0]) += 1/(s*val);
        }
        else
            qDebug() << "Error: Short circuit Inductor!";

    }
    else
    {
        GiNaC::realsymbol val;
        a->makeSymbol(val,value());

        if(idx[0] != 0 && idx[1] != 0)
        {
            a->G(idx[0],idx[0]) += 1/(s*val);
            a->G(idx[1],idx[1]) += 1/(s*val);
            a->G(idx[0],idx[1]) -= 1/(s*val);
            a->G(idx[1],idx[0]) -= 1/(s*val);
        }
        else if(idx[0] == 0 && idx[1] != 0)
        {
            a->G(idx[1],idx[1]) += 1/(s*val);
        }
        else if(idx[1] == 0 && idx[0] != 0)
        {
            a->G(idx[0],idx[0]) += 1/(s*val);
        }
        else
            qDebug() << "Error: Short circuit Inductor!";
    }
}
