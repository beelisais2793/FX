#include "ccvs.h"
#include "analyzer.h"
#include "voltagesource.h"
#include "dbg.h"
#include <sstream>

void CCVS::stamp(Analyzer* a)
{
    if(nodes().size() != 2)
        qDebug() << "Error: Number of CCVS nodes is inconsistent!";

    QStringList nlist;
    nlist << nodes().at(0) << nodes().at(1);
    VoltageSource* v = new VoltageSource(name(), nlist, "0");
    v->setParent(parent());
    v->stamp(a);
    delete v;

    // Find controling voltage in J to stamp
    std::string expected = QString("I(%1)").arg(m_voltageName).toStdString();
    unsigned i = 1;
    for(i = 1; i <= a->M(); ++i)
    {
        std::stringstream stream;
        stream << a->J(i);
        if (expected == stream.str())
        {
            bool ok;
            double val = value().toDouble(&ok);
            if(ok)
            {
                a->D(a->eqnCounter(), i) -= val;
            }
            else
            {
                GiNaC::realsymbol sym;
                a->makeSymbol(sym,value());
                a->D(a->eqnCounter(), i) -= sym;
            }
            break;
        }
    }

    if (i > a->M())
    {
        qDebug() << "Failed to stamp " << name();
    }

}
