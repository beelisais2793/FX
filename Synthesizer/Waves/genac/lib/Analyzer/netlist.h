#ifndef NETLIST_H
#define NETLIST_H

#include "circuit.h"
#include <QtCore/QStack>
#include <QtCore/QStringList>

class QTextStream;
class AbstractCircuit;


class Netlist
{
public:
    Netlist() {}
    void parse(QTextStream & stream,
               const QString & libDir = QString());

    Circuit& circuit()
    { return m_circuit; }

    const QList<QStringList>& transferFunctions() const
    { return m_transferFunctions; }
private:
    enum OperatorType {
        SUBCKT,
        ENDS,
        LIB,
        TF,
        END
    };

    void parse(QTextStream & stream, const QString & libDir,
               bool isLib, AbstractCircuit * circuit);

    OperatorType opType(const QString& str);

    Circuit m_circuit;
    QList<QStringList> m_transferFunctions;
    QStack<OperatorType> m_stack;
};

#endif // NETLIST_H
