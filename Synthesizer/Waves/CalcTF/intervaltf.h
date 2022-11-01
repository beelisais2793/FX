#ifndef INTERVALTF_H
#define INTERVALTF_H
#include "tfcoefficient.h"

#include <QString>
#include <QList>
#include "transferfunction.h"
#include "toolbox.h"


class IntervalTF
{
public:
    IntervalTF();    
    void setN(const QString value);
    void setD(const QString value);
    QString getN();
    QString getD();

    QString getTfEquation(const QString &plane="s");
    QVector<std::shared_ptr<Root>> getRootsClosedLoop(const double K = 1.0);
    QVector<std::shared_ptr<Root>> getRootsClosedLoop(const bool max_K);
    QVector<std::shared_ptr<Root>> getRootLocus();


    void generateTF();
private:
    void purseString(const QString value, QList<TfCoeff> &coeffList);    

    QList<TfCoeff> m_numerator;
    QList<TfCoeff> m_denomenator;

    QVector<TransferFunction> m_tfList;
    void kharitonovP(const QList<TfCoeff> &c, std::vector<double> &v, const int n);
    void generateTF(const QList<TfCoeff> &N, const QList<TfCoeff> &D, QVector<TransferFunction> &Tf);
    void generateTF1(const QList<TfCoeff> &N, const QList<TfCoeff> &D, QVector<TransferFunction> &Tf);
    QString getStr(const QList<TfCoeff> &c);
    QString getCoeffEq(const QList<TfCoeff> &p, const QString &plane);       

};



struct mapTF{
    mapTF(const QList<TfCoeff> &N, const QList<TfCoeff> &D)
    {
        m_N = N;
        m_D = D;
    }

    typedef TransferFunction result_type;

    TransferFunction operator()(TransferFunction &tf)
    {
        std::vector<double> vN;
        std::vector<double> vD;
        ToolBox::generateRandVector(m_N,vN);
        ToolBox::generateRandVector(m_D,vD);

        Polynomial pN = Polynomial(&vN[0],vN.size());
        Polynomial pD = Polynomial(&vD[0],vD.size());
        tf.setTF(pN,pD);

        return tf;
    }
private:
    QList<TfCoeff> m_N,m_D;
};


#endif // INTERVALTF_H
