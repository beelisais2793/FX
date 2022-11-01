#include "cciifilterchromosome.h"
#include "Analyzer/circuit.h"
#include "Analyzer/Primitive/cccs.h"
#include "Analyzer/Primitive/vcvs.h"
#include "Analyzer/Primitive/currentsource.h"
#include "Analyzer/Primitive/voltagesource.h"
#include "Analyzer/Primitive/capacitor.h"
#include "Analyzer/Primitive/resistor.h"
#include "utilities.h"
#include <QtCore/QStringList>

CciiFilterChromosome::CciiFilterChromosome(int ncapacitors, int nresistors)
    :Chromosome(ncapacitors, nresistors)
{
    m_type = Chromosome::CciiFilter;
}

/*virtual*/
QString CciiFilterChromosome::toNetlist() const
{
    CONST_SPLIT_IE(this);
    int ncapacitor = ncapacitors();
    QString str = "Is " + QString::number(isize()) + " "
            + QString::number(_CONST_I(_CONST_E(3))) + " Is\n";
    for(int i = 2; i <= isize(); i += 2) {
        int halfi = i / 2;
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
        QString::number(_CONST_I(_CONST_E(2))) + " " + "CCII\n";

    str += "V1 " + QString::number(isize()) + " 0 0\n";
    str += "V2 " + QString::number(_CONST_I(_CONST_E(2))) + " 0 0\n";
    str += QString(".SUBCKT CCII 1 2 3\n") +
           QString("R_Y 2 0 _inf\n") +
           QString("E_X 1 0 2 0 1\n") +
           QString("F_Z 0 3 E_X 1\n") +
           QString(".ENDS\n");
    str += QString(".TF I(V2) I(V1)\n");
    str += ".END";

    return str;
}

/*virtual*/
int CciiFilterChromosome::esize() const
{
    return 4;
}

/*virtual*/
int CciiFilterChromosome::nShortCircuits() const
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
