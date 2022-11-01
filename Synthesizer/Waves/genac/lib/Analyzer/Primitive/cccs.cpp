#include "cccs.h"
#include "analyzer.h"
#include "dbg.h"
#include <sstream>

void CCCS::stamp(Analyzer* a)
{
    int len = nodes().size();

    if(len != 2)
        qDebug() << "Error: Number of CCCS nodes is inconsistent!";

    uint idx[2];
    for(int i = 0; i < 2; ++i)
    {
        bool ok = true;
        idx[i] = parent()->index(nodes().at(i),&ok);
        if(!ok)
        {
            qDebug() << "Error: Unknown Node!";
            return;
        }
    }

    // Find controling voltage in J to stamp
    std::string expected = QString("I(%1)").arg(m_voltageName).toStdString();
    unsigned i = 1;
    for (unsigned i = 1; i <= a->M(); ++i)
    {
        std::stringstream stream;
        stream << a->J(i);
        if (expected == stream.str())
        {
            bool ok = true;
            double val = value().toDouble(&ok);
            if(ok)
            {
                if(idx[0] != 0)
                    a->B(idx[0], i) += val;
                if(idx[1] != 0)
                    a->B(idx[1], i) += -1*val;
            }
            else
            {
                GiNaC::realsymbol sym;
                a->makeSymbol(sym,value());
                if(idx[0] != 0)
                    a->B(idx[0], i) += sym;
                if(idx[1] != 0)
                    a->B(idx[1], i) += -1*sym;
            }
            break;
        }
    }

    if (i > a->M())
    {
        qDebug() << "Failed to stamp " << name();
    }
}
