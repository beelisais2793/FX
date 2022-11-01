#include "opampfilterchromosome.h"
#include "Analyzer/circuit.h"
#include "Analyzer/Primitive/vcvs.h"
#include "Analyzer/Primitive/voltagesource.h"
#include "Analyzer/Primitive/capacitor.h"
#include "Analyzer/Primitive/resistor.h"
#include "utilities.h"
#include <QtCore/QStringList>

OpampFilterChromosome::OpampFilterChromosome(int ncapacitors, int nresistors)
    :Chromosome(ncapacitors, nresistors)
{
    m_type = Chromosome::OpampFilter;
}

/*virtual*/
QString OpampFilterChromosome::toNetlist() const
{
    CONST_SPLIT_IE(this);
    int ncapacitor = ncapacitors();
    QString str = "Vs " + QString::number(_CONST_I(_CONST_E(3))) + " " + "0 Vs\n";
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
                QString::number(_CONST_I(i)) + " " + name + "\n";
        }
    }
    str += "X1 " + QString::number(_CONST_I(_CONST_E(0))) + " " +
        QString::number(_CONST_I(_CONST_E(1))) + " " +
        QString::number(_CONST_I(_CONST_E(2))) + " " + "OPAMP\n";

    str += QString(".SUBCKT OPAMP 1 2 3\n") +
           QString("E_X 3 0 1 2 _inf\n") +
           QString(".ENDS\n");
    str += QString(".TF V(%1) V(%2)\n").arg(_CONST_I(_CONST_E(2)))
                                       .arg(_CONST_I(_CONST_E(3)));
    str += ".END";
    return str;
}

/*virtual*/
int OpampFilterChromosome::esize() const
{
    return 4;
}

/*virtual*/
int OpampFilterChromosome::nShortCircuits() const
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
