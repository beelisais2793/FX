#ifndef ESTIMATOR_H
#define ESTIMATOR_H

#include <QtCore/QVector>
#include <ginac/ginac.h>
#include <QtCore/QString>

class Chromosome;
class Population;
class Analyzer;

class Estimator
{
public:
    Estimator(const QString& requirements = QString());
    ~Estimator();
    void setQuality(Chromosome *c);
    void setPopulationData(Population *p);
    void setRequirements(const QString& requirements);
    QVector<int> requirements() {return m_requirements;}
private:
    Analyzer *m_analyzer;
    int m_nexception;
    QVector<unsigned long long> m_sums;
    GiNaC::realsymbol m_s;
    GiNaC::realsymbol m_inf;
    QVector<int> m_requirements;
};



#endif /* ESTIMATOR_H */
