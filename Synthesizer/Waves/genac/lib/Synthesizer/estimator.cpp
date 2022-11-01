#include "estimator.h"
#include "chromosome.h"
#include "population.h"
#include "dbg.h"
#include "Analyzer/netlist.h"
#include "Analyzer/analyzer.h"
#include "Analyzer/circuit.h"
#include "Analyzer/pretty.h"
#include <cfloat>

# define ALPHA (0.60f)
# define BETA  (0.40f)

#define VALID_FUNC(cost)   (BETA + ALPHA / (1.0f + (float)(cost)))
#define INVALID_FUNC(cost) (BETA / (1.0f + (float)(cost)))

Estimator::Estimator(const QString& requirements)
    :m_analyzer(new Analyzer),
     m_nexception(0)
{
    m_analyzer->makeSymbol(m_s,"s");
    m_analyzer->makeSymbol(m_inf,"_inf");
    setRequirements(requirements);
}

Estimator::~Estimator()
{
    delete m_analyzer;
}

void Estimator::setRequirements(const QString& requirements)
{
    m_requirements.resize(requirements.size() + 2);
    int i = 0;
    for (i = 0; i < requirements.size(); ++i) {
        if (requirements[i] == '1') {
            m_requirements[i] = 1;
        } else {
            m_requirements[i] = 0;
        }
    }

    int half = requirements.size() / 2;
    QString numerator = requirements.left(half);
    QString denominator = requirements.mid(half);

    m_requirements[i++] = half - numerator.indexOf('1') - 1;
    m_requirements[i] = half - denominator.indexOf('1') - 1;
    m_sums.resize(m_requirements.size());
}

void Estimator::setQuality(Chromosome *c)
{
    if (!c->transferFunction().isEmpty()) {
        return;
    }

    int cost = c->nShortCircuits() + c->nOpenCircuits();
    if (cost > 0) {
        c->setQuality(INVALID_FUNC(cost));
        c->setTransferFunction("Has Open or Short Elements");
        return;
    }

    Netlist netlist;
    QString netlistString = c->toNetlist();
    QTextStream stream(&netlistString);
    netlist.parse(stream);
    Circuit *circuit = &netlist.circuit();
    m_analyzer->stamp(circuit);
    Analyzer::TransferFunction tf;
    try {
        m_analyzer->solve();
        tf = m_analyzer->calcTFs(netlist.transferFunctions())[0];
        c->setTransferFunction(pretty(tf));
    } catch (const std::exception &e) {
        ++m_nexception;
        qDebug() << QString::fromStdString(e.what())
                 << " exception: " << m_nexception
                 << " " << c->toPrintable();
        c->setQuality(FLT_EPSILON);
        c->setTransferFunction("Makes an inconsistent matrix");
        return;
    }

    if (tf.rhsNum.is_zero()
        || tf.rhsDen.is_zero()
        || tf.rhsNum.is_equal(tf.rhsDen)) {
        c->setQuality(BETA);
        return;
    }

    int reqsize = m_requirements.size();
    int degree = reqsize / 2 - 2;
    GiNaC::ex coeff;
    bool bad = false;
    QVector<bool> imperfections;
    imperfections.resize(reqsize);
    cost = 0;
    for (int i = degree; i >= 0; --i) {
        coeff = tf.rhsNum.coeff(m_s, i);
        bad = coeff.is_zero() == m_requirements[degree - i];
        imperfections[degree - i] = bad;
        cost += bad;

    }
    for (int i = degree; i >= 0; --i) {
        coeff = tf.rhsDen.coeff(m_s, i);
        bad = coeff.is_zero() == m_requirements[2*degree - i + 1];
        imperfections[2*degree - i + 1] = bad;
        cost += bad;
    }

    bad = tf.rhsNum.degree(m_s) != m_requirements[reqsize - 2];
    imperfections[reqsize - 2] = bad;
    cost += bad;
    bad = tf.rhsDen.degree(m_s) != m_requirements[reqsize - 1];
    imperfections[reqsize - 1] = bad;
    cost += bad;

    c->setImperfections(imperfections);
    c->setQuality(VALID_FUNC(cost));
}

void Estimator::setPopulationData(Population *p)
{
    Q_CHECK_PTR(p);
    const int M = p->size();
    Q_ASSERT(M > 0);
    int L = p->at(0)->size();
    const int R = m_requirements.size();
    float diversity = 0.0f;
    Chromosome *ci = nullptr;
    Chromosome *cj = nullptr;
    for (int i = 0; i < M-1; ++i) {
        ci = (*p)[i];
        for (int j = i + 1; j < M; ++j) {
            cj = (*p)[j];
            diversity += ci->hammingDistance(*cj);
        }
        setQuality(ci);
        if (ci->isValid()) {
            for (int k = 0; k < R; ++k) {
                    m_sums[k] += ci->hasImperfection(k);
            }
        }
    }
    ci = (*p)[M-1];
    setQuality(ci);
    for (int k = 0; k < R; ++k) {
        if (ci->isValid()) {
            m_sums[k] += ci->hasImperfection(k);
        }
    }

    qDebug() << "Imperfection sums: " << m_sums;

    diversity /= (L * M * (M - 1)/2.0f);
    p->setDiversity(diversity);
    float cost = 0.0f;
    int nimperfections = 0;
    float avrgq = 0.0f;
    float maxq = 0.0f;
    float q = 0.0f;
    for (int i = 0; i < M; ++i) {
        ci = (*p)[i];
        q = ci->quality();
        if (q > maxq) {
            maxq = q;
        }
        cost = 0.0f;
        nimperfections = 0;
        avrgq += q;
        if (ci->isValid()) {
            for (int k = 0; k < R; ++k) {
                cost += m_sums[k] * ci->hasImperfection(k);
                nimperfections += ci->hasImperfection(k);
            }
            cost *= (float)nimperfections/(R * R);
            ci->setFitness(VALID_FUNC(cost));
        } else {
            ci->setFitness(q);
        }
    }

    avrgq /= M;
    p->setAverageQuality(avrgq);
    p->setMaxQuality(maxq);

    float maxf = 0.0f;
    float avrgf = 0.0f;
    float f = 0.0f;
    for (int i = 0; i < M; ++i) {
        ci = (*p)[i];
        f = ci->fitness();
        if (f > maxf) {
            maxf = f;
        }
        avrgf += f;
    }
    avrgf /= M;
    p->setAverageFitness(avrgf);
    p->setMaxFitness(maxf);
}
