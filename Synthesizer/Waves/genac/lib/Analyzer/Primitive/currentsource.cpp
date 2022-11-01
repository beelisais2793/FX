#include "currentsource.h"
#include "analyzer.h"
#include "dbg.h"

void CurrentSource::stamp(Analyzer* a)
{
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

    double num = value().toDouble(&ok);
    if(ok)
    {
        GiNaC::numeric val(num);

        if(idx[0] != 0 && idx[1] != 0)
        {
            a->I(idx[0]) -= val;
            a->I(idx[1]) += val;
        }
        else if(idx[0] != 0)
        {
            a->I(idx[0]) -= val;
        }
        else if(idx[1] != 0)
        {
            a->I(idx[1]) += val;
        }
        else
        {
            qDebug() << "Error: Short circuit Indepentdent Current Source!";
        }

    }
    else
    {
        GiNaC::realsymbol val;
        a->makeSymbol(val, value());
        if(idx[0] != 0 && idx[1] != 0)
        {
            a->I(idx[0]) -= val;
            a->I(idx[1]) += val;
        }
        else if(idx[0] != 0)
        {
            a->I(idx[0]) -= val;
        }
        else if(idx[1] != 0)
        {
            a->I(idx[1]) -= val;
        }
        else
        {
            qDebug() << "Error: Short circuit Indepentdent Current Source!";
        }
    }
}
