#include "vccs.h"
#include "analyzer.h"
#include "mapper.h"
#include "dbg.h"


void VCCS::stamp(Analyzer* a)
{
    int len = nodes().size();

    if(len != 4)
    {
        qDebug() << "Error: Number of VCVS nodes is inconsistent!";
    }

    unsigned idx[len];
    bool ok = true;
    for(int i = 0; i < len; ++i)
    {
        idx[i] = parent()->index(nodes().at(i), &ok);
        if(!ok)
        {
            qDebug() << "Error: Unknown Node!";
            return;
        }
    }

    double val = value().toDouble(&ok);
    if(ok)
    {
        if(idx[0] != 0 && idx[1] != 0 && idx[2] != 0 && idx[3] != 0)
        {
            a->G(idx[0],idx[2]) += val;
            a->G(idx[1],idx[3]) += val;
            a->G(idx[0],idx[3]) -= val;
            a->G(idx[1],idx[2]) -= val;
        }
        else if(idx[0] == 0 && idx[1] != 0)
        {
            a->G(idx[1],idx[2]) -= val;
            a->G(idx[1],idx[3]) += val;
        }
        else if(idx[1] == 0 && idx[0] != 0)
        {
            a->G(idx[0],idx[2]) += val;
            a->G(idx[0],idx[3]) -= val;
        }
        else if(idx[2] == 0 && idx[3] != 0)
        {
            a->G(idx[0],idx[3]) -= val;
            a->G(idx[1],idx[3]) -= val;
        }
        else if(idx[3] == 0 && idx[2] != 0)
        {
            a->G(idx[0],idx[2]) += val;
            a->G(idx[1],idx[2]) -= val;
        }
        else
        {
            qDebug()<<"Error: Short circuit VCCS!";
        }
    }
    else
    {
        GiNaC::realsymbol val;
        a->makeSymbol(val,value());

        if(idx[0] != 0 && idx[1] != 0 && idx[2] != 0 && idx[3] != 0)
        {
            a->G(idx[0],idx[2]) += val;
            a->G(idx[1],idx[3]) += val;
            a->G(idx[0],idx[3]) -= val;
            a->G(idx[1],idx[2]) -= val;
        }
        else if(idx[0] == 0 && idx[1] != 0)
        {
            a->G(idx[1],idx[2]) -= val;
            a->G(idx[1],idx[3]) += val;
        }
        else if(idx[1] == 0 && idx[0] != 0)
        {
            a->G(idx[0],idx[2]) += val;
            a->G(idx[0],idx[3]) -= val;
        }
        else if(idx[2] == 0 && idx[3] != 0)
        {
            a->G(idx[0],idx[3]) -= val;
            a->G(idx[1],idx[3]) -= val;
        }
        else if(idx[3] == 0 && idx[2] != 0)
        {
            a->G(idx[0],idx[2]) += val;
            a->G(idx[1],idx[2]) -= val;
        }
        else
        {
            qDebug()<<"Error: Short circuit VCCS!";
        }
    }
}
