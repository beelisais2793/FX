#include "otrafilterchromosome.h"
#include "Analyzer/circuit.h"
#include "Analyzer/Primitive/cccs.h"
#include "Analyzer/Primitive/vcvs.h"
#include "Analyzer/Primitive/voltagesource.h"
#include "Analyzer/Primitive/capacitor.h"
#include "Analyzer/Primitive/resistor.h"
#include "utilities.h"
#include <QtCore/QStringList>

OtraFilterChromosome::OtraFilterChromosome(int ncapacitors, int nresistors)
    :Chromosome(ncapacitors, nresistors)
{
    m_type = Chromosome::OtraFilter;
}

/*virtual*/
QString OtraFilterChromosome::toNetlist() const
{
    CONST_SPLIT_IE(this);
    int ncapacitor = ncapacitors();
    QString str = "Vs " + QString::number(_CONST_I(_CONST_E(3))) + " 0 Vs\n";
    for(int i = 2; i <= isize(); i += 2) {
        int halfi = i/2;
        if(halfi <= ncapacitor) {
            QString name = "C" + QString::number(halfi);
            str += name + " " +
                QString::number(_CONST_I(i-1)) + " " +
                QString::number(_CONST_I(i)) + " " + name +"\n";
        } else {
            QString name = "R" + QString::number(halfi - ncapacitor);
            str +=  name + " " +
                QString::number(_CONST_I(i-1)) + " " +
                QString::number(_CONST_I(i)) + " " + name +"\n";
        }
    }
    str += "X1 " + QString::number(_CONST_I(_CONST_E(0))) + " " +
        QString::number(_CONST_I(_CONST_E(1))) + " " +
        QString::number(_CONST_I(_CONST_E(2))) + " " + "OTRA\n";

    str += QString(".SUBCKT OTRA 1 2 3\n") +
           QString("V_X 1 0 0\n") +
           QString("V_Y 2 0 0\n") +
           QString("F_X 0 hidden V_X 1\n") +
           QString("F_Y 0 hidden V_Y -1\n") +
           QString("R_Z hidden 0 _inf\n") +
           QString("E_Z 3 0 hidden 0 1\n") +
           QString(".ENDS\n");
    str += QString(".TF V(%1) V(%2)\n").arg(_CONST_I(_CONST_E(2)))
                                       .arg(_CONST_I(_CONST_E(3)));
    str += ".END";

    return str;
}

/*virtual*/
int OtraFilterChromosome::esize() const
{
    return 4;
}

/*virtual*/
int OtraFilterChromosome::nShortCircuits() const
{
    CONST_SPLIT_IE(this);
    int n = Chromosome::nShortCircuits();
    if (_CONST_I(_CONST_E(0)) == _CONST_I(_CONST_E(1))) {
        ++n;
    }
    if (_CONST_I(_CONST_E(0)) == _CONST_I(_CONST_E(3))) {
        ++n;
    }
    if (_CONST_I(_CONST_E(1)) == _CONST_I(_CONST_E(3))) {
        ++n;
    }
    if (_CONST_I(_CONST_E(2)) == _CONST_I(_CONST_E(3))) {
        ++n;
    }

    for (int i = 0; i < esize(); ++i) {
        if (_CONST_I(_CONST_E(i)) == 0) {
            ++n;
        }
    }
    return n;
}
