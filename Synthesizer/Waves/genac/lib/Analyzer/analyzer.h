#ifndef ANALYZER_H
#define ANALYZER_H
#include <ginac/ginac.h>
#include <QtCore/QString>
#include "syms.h"

class Circuit;

struct Equation
{
    GiNaC::ex rhsNum;
    GiNaC::ex rhsDen;
    GiNaC::ex lhsNum;
    GiNaC::ex lhsDen;
};

class Analyzer
{
public:
    explicit Analyzer(Circuit* circuit = 0);
    ~Analyzer();
    uint& eqnCounter();
    uint& N() const; //return number of total nodes.
    uint& M() const; //return number of extra equations.
    void stamp(Circuit* circuit);
    void solve();

    typedef Equation TransferFunction;

    GiNaC::ex& A(uint i,uint j);
    GiNaC::ex& G(uint i,uint j);
    GiNaC::ex& C(uint i,uint j);
    GiNaC::ex& B(uint i,uint j);
    GiNaC::ex& D(uint i,uint j);

    GiNaC::ex& X(uint i);
    GiNaC::ex& V(uint i);
    GiNaC::ex& J(uint i);

    GiNaC::ex& Z(uint i);
    GiNaC::ex& I(uint i);
    GiNaC::ex& E(uint i);



    void makeSymbol(GiNaC::realsymbol& symbl, const QString& name)
    {
        m_symbols.getSymbol(symbl,name);
    }

    Syms& symbols()
    {
        return m_symbols;
    }


    GiNaC::ex findValue(const QString& exp, bool *found);
    TransferFunction calcTF(const QString& out, const QString& in);
    QList<TransferFunction> calcTFs(const QList<QStringList>& tfs);
private:
     void destroy();

     Circuit* m_circuit;

     GiNaC::matrix* m_A;
     GiNaC::matrix* m_X;
     GiNaC::matrix* m_Z;
     GiNaC::matrix m_result;
     uint m_eqnCounter;
     Syms m_symbols;

};

#endif // ANALYZER_H
