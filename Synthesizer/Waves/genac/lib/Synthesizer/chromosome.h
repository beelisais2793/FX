#ifndef CHROMOSOME_H
#define CHROMOSOME_H

#include <QtCore/QString>
#include <QtCore/QVector>

#define SPLIT_IE(ptr) Chromosome& __r = *(ptr)
#define _I(i) (__r[(i)])
#define _E(i) (__r[__r.isize() + (i)])

#define CONST_SPLIT_IE(cptr) const Chromosome& __cr = *(cptr)
#define _CONST_I(i) (__cr[(i)])
#define _CONST_E(i) (__cr[__cr.isize() + (i)])

class Circuit;

// I-type terminals: Ground and passive elements' terminals.
// E-type terminals: Terminal of the independent
// source and active element's terminals.

class Chromosome
{
public:
    enum Type {
        OpampFilter,
        OtraFilter,
        CciiFilter,
        CfoaFilter
    };

    Chromosome(int ncapacitors, int nresistors);
    Chromosome(const Chromosome&) =delete;
    Chromosome& operator=(const Chromosome&) =delete;

    virtual ~Chromosome() =default;

    Type type() const
    {return m_type;}

    int nresistors() const
    { return m_nresistor; }

    int ncapacitors() const
    { return m_ncapacitor; }

    int isize() const
    { return (m_nresistor + m_ncapacitor) * 2 + 1; }

    int imax() const
    { return m_nresistor + m_ncapacitor + 1;}

    int imin() const {return 0;}

    int emax() const
    { return isize() - 1;}

    int emin() const {return 0;}

    QString transferFunction() const
    {return m_transferFunction;}
    void setTransferFunction(const QString& tf)
    {m_transferFunction = tf;}

    void setQuality(float quality)
    {m_quality = quality;}
    float quality() const
    {return  m_quality;}

    void setFitness(float fitness)
    {m_fitness = fitness;}

    float fitness() const;

    void penalize()
    {++m_penalty;}

    bool isValid() const
    {return !m_imperfections.isEmpty();}
    bool hasImperfection(int i) const
    {return isValid() && m_imperfections[i];}
    QVector<bool> imperfections() const
    {return m_imperfections;}
    void setImperfections(const QVector<bool>& v)
    {m_imperfections = v;}

    int at(int i) const {return m_IE.at(i);}
    int operator[](int i) const {return m_IE[i];}
    int& operator[](int i) {return m_IE[i];}
    int indexOf(int node) const {return m_IE.indexOf(node);}


    int size() const;
    int hammingDistance(const Chromosome& other) const;
    int similarity(const Chromosome& other) const;
    QString toPrintable() const;

    void fill();
    void canonicalize();

    virtual int nOpenCircuits() const;
    virtual int nShortCircuits() const;

    virtual int esize() const =0;
    virtual QString toNetlist() const =0;

    static Chromosome* create(Type type, int ncapacitors, int nresistors,
                              bool random = true);
    static Chromosome* create(Type type, int ncapacitors, int nresistors,
                              const QVector<int>& IE);
    static Chromosome* clone(const Chromosome& other);

protected:
    Type m_type;

private:
    int m_ncapacitor;
    int m_nresistor;
    int m_penalty;
    float m_quality;
    float m_fitness;
    QString m_transferFunction;
    QVector<bool> m_imperfections;
    QVector<int> m_IE;
};

#endif // CHROMOSOME_H
