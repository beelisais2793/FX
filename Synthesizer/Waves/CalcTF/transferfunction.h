#ifndef TRANSFERFUNCTION_H
#define TRANSFERFUNCTION_H

#include "Polynomial.h"
#include "root.h"

#include <QList>
#include <QString>
#include <memory>



class TransferFunction
{
public:
    TransferFunction();
    TransferFunction(double scalar);
    TransferFunction(TransferFunction *tf0);
    TransferFunction(const Polynomial &zeroP, const Polynomial &poleP);
    TransferFunction(const QString &zeroStr, const QString &poleStr);
    TransferFunction(const std::string &zeroStr, const std::string &poleStr);

    void setTF(const QString &zeroStr, const QString &poleStr);
    void setTF(const Polynomial &zeroP, const Polynomial &poleP);

    QString setZerosPoly(const QString &polyStr,QString *errString = nullptr);
    QString setPolesPoly(const QString &polyStr,QString *errString = nullptr);
//    QString getPolesStr();
//    QString getZeroStr();
    QString getPolesVectorStr();
    QString getZeroVectorStr();


    std::shared_ptr<Polynomial> zerosPoly() const;
    void setZerosPoly(std::shared_ptr<Polynomial> &zerosPoly);

    std::shared_ptr<Polynomial> polesPoly() const;
    void setPolesPoly(std::shared_ptr<Polynomial> &polesPoly);    

    QString getTfEquation(const QString &plane="s");
    QVector<std::shared_ptr<Root>> getRootsClosedLoop(const double K);
    QVector<std::shared_ptr<Root>> getRootsClosedLoop(const bool max_K) const;

    bool isEmpty();

    void simplifyTF();
    std::complex<double> dumpKValues(double xr, double xi);
    QVector<std::shared_ptr<Root>> getRootLocus() const;

    void setMaxK(bool value){m_maxK = value;}
    bool getMaxK() const {return m_maxK;}

    QVector<std::shared_ptr<Root>> getRootsClosedLoop1() const;

private:


    std::shared_ptr<Polynomial> m_zerosPoly;
    std::shared_ptr<Polynomial> m_polesPoly;
    bool m_maxK;
    QString setPolynomialFomStr(const QString &polyStr, std::shared_ptr<Polynomial> &p, QString *errString = nullptr);
    QString getPolynomialStr(QString &polyStr, const std::shared_ptr<Polynomial> p);
//    QString getPolynomialStr(const std::shared_ptr<Polynomial> p);
    QString getPolynomialEquation(const std::shared_ptr<Polynomial> &p, const QString &plane);
    QString getPolynomialVectorStr(const std::shared_ptr<Polynomial> &p);
    QString getPolynomialVectorStr(const Polynomial &p);    
    void initTS();
    double getRootAt(QVector<double> &vect, int i);
    QVector<std::shared_ptr<Root>> getRoots(Polynomial &P) const;

    QVector<std::shared_ptr<Root>> getRootsClosedLoop(Polynomial &N, Polynomial &D, const double K) const;
    std::complex<double> evaluateComplex(const Polynomial &P, std::complex<double> p);
    unsigned int factorial(unsigned int n);

    int getClosestRoot(QVector<std::shared_ptr<Root>> &roots, std::complex<double> &root) const;
    QVector<std::shared_ptr<Root>> getRootLocus(Polynomial &N, Polynomial &D, double &K_max, QList<QVector<std::shared_ptr<Root>>> &locus) const;

};

TransferFunction operator *(const TransferFunction &tr0, const TransferFunction &tr1);
TransferFunction operator /(const TransferFunction &tr0, const TransferFunction &tr1);
TransferFunction operator +(const TransferFunction &tr0, const TransferFunction &tr1);
TransferFunction operator -(const TransferFunction &tr0, const TransferFunction &tr1);


#endif // TRANSFERFUNCTION_H
