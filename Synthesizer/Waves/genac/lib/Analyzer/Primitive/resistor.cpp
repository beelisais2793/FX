#include "resistor.h"
#include "mapper.h"
#include "analyzer.h"
#include "dbg.h"

void Resistor::stamp(Analyzer *a)
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
                a->G(idx[0],idx[0]) += 1/val;
                a->G(idx[1],idx[1]) += 1/val;
                a->G(idx[0],idx[1]) -= 1/val;
                a->G(idx[1],idx[0]) -= 1/val;
            }
            else if(idx[0] == 0 && idx[1] == 0)
            {
                a->G(idx[1],idx[1]) += 1/val;
            }
            else if(idx[1] == 0 && idx[0] != 0)
            {
                a->G(idx[0],idx[0]) += 1/val;
            }
            else
                qDebug() << "Error: Short circuit Resistor!";

        }
        else
        {
            GiNaC::realsymbol val;
            a->makeSymbol(val,value());

            if(idx[0] != 0 && idx[1] != 0)
            {
                a->G(idx[0],idx[0]) += 1/val;
                a->G(idx[1],idx[1]) += 1/val;
                a->G(idx[0],idx[1]) -= 1/val;
                a->G(idx[1],idx[0]) -= 1/val;
            }
            else if(idx[0] == 0 && idx[1] != 0)
            {
                a->G(idx[1],idx[1]) += 1/val;
            }
            else if(idx[1] == 0 && idx[0] != 0)
            {
                a->G(idx[0],idx[0]) += 1/val;
            }
            else
                qDebug() << "Error: Short circuit Resistor!";
        }
}
