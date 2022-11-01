#include "analyzer.h"
#include "circuit.h"
#include "primitive.h"
#include <sstream>
#include <iostream>
#include "dbg.h"

static void print_matrix(const std::string& lhs, const GiNaC::matrix& matrix);

Analyzer::Analyzer(Circuit* circuit)
    :m_circuit(0),m_A(0),m_X(0),m_Z(0),m_eqnCounter(0)
{
        if(circuit){
        stamp(circuit);
        }
}

Analyzer::~Analyzer()
{
    destroy();
}

uint& Analyzer::eqnCounter()
{
    return m_eqnCounter;
}

uint& Analyzer::N() const
{
    if(!m_circuit)
        qDebug() << "Error: NULL Circuit in Analyzer N!";

        return m_circuit->N();
}

uint& Analyzer::M() const
{
    if(!m_circuit)
        qDebug() << "Error: NULL Circuit in Analyzer M!";

    return m_circuit->M();
}

void Analyzer::stamp(Circuit* circuit)
{
    if(m_circuit)
    {
        destroy();
    }

    m_circuit = circuit;

    if(m_circuit)
    {
        uint n = N();
        uint m = M();
        m_A = new GiNaC::matrix(n+m,n+m);
        m_X = new GiNaC::matrix(n+m,1);
        m_Z = new GiNaC::matrix(n+m,1);

        for(uint i = 1; i<=N(); ++i)
        {
            this->X(i) = GiNaC::realsymbol("V("+ m_circuit->getIndexedNode(i).toStdString()+")");
        }


        m_circuit->do_stamp(this);
    }
    else
    {
        qDebug() << "error at stamp";
        return;
    }
}

void Analyzer::solve()
{
    // print_matrix("A", *m_A);
    // print_matrix("x", *m_X);
    // print_matrix("z", *m_Z);
    m_result = m_A->solve(*m_X,*m_Z, GiNaC::solve_algo::automatic);
    // print_matrix("solution", m_result);
}

void Analyzer::destroy()
{
    delete m_A; m_A = 0;
    delete m_X; m_X = 0;
    delete m_Z; m_Z = 0;
    m_circuit = 0;
    m_eqnCounter = 0;
}

GiNaC::ex& Analyzer::A(uint i,uint j)
{
    uint n = N();
    uint m = M();

    if(i < 1 || j <1 || i> m+n || j > m+n)
    {
        qDebug() << "Index Error at A";
    }
    --i;
    --j;

    return (*m_A)(i,j);
}

GiNaC::ex& Analyzer::G(uint i,uint j)
{
    uint n = N();

    if(i < 1 || j < 1 || i>n || j>n )
       {
           qDebug() << "Index Error at G";
       }
      --i;
      --j;

       return (*m_A)(i,j);
}

GiNaC::ex& Analyzer::C(uint i,uint j)
{
    uint n = N();
    uint m = M();

    if(i < 1 || j < 1 || i > m || j > n)
    {
        qDebug() << "Index Error at C";
    }

    i += (n-1);
  --j;

    return (*m_A)(i,j);
}

GiNaC::ex& Analyzer::B(uint i,uint j)
{
    uint n = N();
    uint m = M();

    if(i < 1 || j < 1 || i > n || j > m)
    {
        qDebug() << "Index Error at B";
    }
    --i;
      j += (n-1);
    return (*m_A)(i,j);
}

GiNaC::ex& Analyzer::D(uint i,uint j)
{
    uint n = N();
    uint m = M();

    if(i < 1 || j < 1 || i > m || j > m)
    {
        qDebug() << "Index Error at D";
    }

    i += (n-1);
    j += (n-1);

    return (*m_A)(i,j);
}

GiNaC::ex& Analyzer::X(uint i)
{
    uint n = N();
    uint m = M();

    if(i < 1 || i > m+n)
    {
        qDebug() << "Index Error at X";
    }

    --i;

    return (*m_X)(i,0);
}

GiNaC::ex& Analyzer::V(uint i)
{
    uint n = N();

    if(i  < 1 || i > n)
    {
     qDebug() << "Index Error at V";
    }

    --i;

    return (*m_X)(i,0);
}

GiNaC::ex& Analyzer::J(uint i)
{
    uint n = N();
    uint m = M();

    if(i < 1 || i > m)
    {
     qDebug() << "Index Error at J";
    }

    i += (n-1);

    return (*m_X)(i,0);
}

GiNaC::ex& Analyzer::Z(uint i)
{
    uint n = N();
    uint m = M();

    if(i < 1 || i > m+n)
    {
     qDebug() << "Index Error at Z";
    }
    --i;

    return (*m_Z)(i,0);
}

GiNaC::ex& Analyzer::I(uint i)
{
    uint n = N();

    if(i  < 1 || i > n)
    {
     qDebug() << "Index Error at I";
    }

    --i;

    return (*m_Z)(i,0);
}

GiNaC::ex& Analyzer::E(uint i)
{
    uint n = N();
    uint m = M();

    if(i < 1 || i > m)
    {
     qDebug() << "Index Error at E";
    }

    i += (n-1);

    return (*m_Z)(i,0);
}

GiNaC::ex Analyzer::findValue(const QString& exp, bool *found)
{
    // Find V(x) or I(x) in X
     unsigned i = 0;
     *found = false;
     for(i = 0; i < m_X->rows(); ++i)
     {
        std::stringstream stream;
         stream << (*m_X)(i, 0);

         if(exp.toStdString() == stream.str())
         {
             *found = true;
             return m_result(i, 0);
         }
     }
     return m_result(0, 0);
}

Analyzer::TransferFunction Analyzer::calcTF(const QString& out, const QString& in)
{
    GiNaC::ex num, den, num_den;
    TransferFunction tf;
    bool found = false;
    const GiNaC::realsymbol inf = m_symbols.getInf();
    const GiNaC::realsymbol s = m_symbols.getS();

    num = findValue(out, &found);
    den = findValue(in, &found);

    num_den = (num / den).numer_denom();

    tf.rhsNum = num_den.op(0).lcoeff(inf).expand().collect(s);
    tf.rhsDen = num_den.op(1).lcoeff(inf).expand().collect(s);

    tf.lhsNum = GiNaC::realsymbol(out.toStdString());
    tf.lhsDen = GiNaC::realsymbol(in.toStdString());
    return tf;
}

QList<Analyzer::TransferFunction> Analyzer::calcTFs(const QList<QStringList>& tfs)
{
    QList<TransferFunction> list;

    for (int i = 0; i < tfs.size(); ++i)
    {
        list.append(calcTF(tfs.at(i).at(0), tfs.at(i).at(1)));
    }

    return list;
}

static void print_matrix(const std::string& lhs, const GiNaC::matrix& matrix)
{
    std::cout << lhs << " = " << std::endl;
    for (uint i = 0; i < matrix.rows(); ++i) {
        for (uint j = 0; j < matrix.cols(); ++j) {
            std::cout << matrix(i, j) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
