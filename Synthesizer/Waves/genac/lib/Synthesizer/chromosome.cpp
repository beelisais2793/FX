#include "chromosome.h"
#include "opampfilterchromosome.h"
#include "otrafilterchromosome.h"
#include "cciifilterchromosome.h"
#include "cfoafilterchromosome.h"
#include "utilities.h"
#include <QtCore/QMap>
#include <cmath>
#include "dbg.h"

#define PENALIZED(f, p) (f * powf(M_E, (-1.0f*p)))

Chromosome::Chromosome(int ncapacitors, int nresistors)
    :m_type{OpampFilter}, m_ncapacitor{ncapacitors}, m_nresistor{nresistors},
     m_penalty(0), m_quality(0.0f), m_fitness(0.0f)
{
}

int Chromosome::size() const
{
    return isize() + esize();
}

void Chromosome::fill()
{
    m_IE.resize(size());
}


float Chromosome::fitness() const
{
    return  PENALIZED(m_fitness, m_penalty);
}

/*virtual*/
int Chromosome::nOpenCircuits() const
{
    CONST_SPLIT_IE(this);
    int cnt = 0;
    int imx = imax();
    int *temp = new int[imx + 1];

    for(int i = 0; i <= imx; ++i) {
        temp[i] = 0;
    }
    for(int i = 0; i < size(); ++i) {
        if(i < isize()) {
            ++temp[at(i)];
        } else {
            ++temp[_CONST_I(at(i))];
        }
    }
    for(int i = 0; i <= imx; ++i) {
        if(temp[i] == 1) {
            ++cnt;
        }
    }

    delete[] temp;
    return cnt;
}

/*virtual*/
int Chromosome::nShortCircuits() const
{
    CONST_SPLIT_IE(this);
    int cnt = 0;
    for(int i = 1; i < isize(); ++i) {
        if(i%2 == 0 && _CONST_I(i) == _CONST_I(i-1)) {
            ++cnt;
        }
    }

    return cnt;
}

int Chromosome::hammingDistance(const Chromosome& other) const
{
    int d = 0;
    int len = size();

    for (int i = 0; i < len; ++i) {
        if (at(i) != other.at(i)) {
            ++d;
        }
    }
    return d;
}

int Chromosome::similarity(const Chromosome& other) const
{
    return size() - hammingDistance(other);
}

QString Chromosome::toPrintable() const
{
    QString str = "";
    int len = size();

    for (int i = 0; i < len; ++i) {
        str += QString::number(at(i));
        if (i < len - 1) {
            str += " ";
        }
    }
    return str;
}

Chromosome* Chromosome::create(Type type, int ncapacitors,
                               int nresistors, bool random)
{
    Chromosome *c = nullptr;
    switch (type) {
    case Chromosome::OpampFilter:
        c = new OpampFilterChromosome(ncapacitors, nresistors);
        break;
    case Chromosome::OtraFilter:
        c = new OtraFilterChromosome(ncapacitors, nresistors);
        break;
    case Chromosome::CciiFilter:
        c = new CciiFilterChromosome(ncapacitors, nresistors);
        break;
    case Chromosome::CfoaFilter:
        c = new CfoaFilterChromosome(ncapacitors, nresistors);
        break;
    }

    c->fill();
    if (random) {
        SPLIT_IE(c);
        // Random generate
        for (int i = c->emin(); i < c->esize(); ++i) {
            _E(i) = randomInt(c->emin(), c->emax());
        }
        // _I(0) is always Ground
        for (int i = 1; i <= c->isize(); ++i) {
            _I(i) = randomInt(c->imin(), c->imax());
            if (i % 2 == 0 && _I(i) == _I(i-1)) {
                if (_I(i) > 0) {
                    _I(i) -= 1;
                } else if (_I(i) < c->imax()) {
                    _I(i) += 1;
                }
            }
        }
        c->canonicalize();
    }

    return c;
}

Chromosome* Chromosome::create(Type type, int ncapacitors,
                               int nresistors, const QVector<int>& IE)
{
    Chromosome *c = Chromosome::create(type, ncapacitors, nresistors, false);

    Q_ASSERT_X(c->size() == IE.size(),
               "Chromosome::create",
               "IE vector and chromosome size don't agree");

    for (int i = 0; i < IE.size(); ++i) {
        (*c)[i] = IE[i];
    }

    c->canonicalize();
    return c;
}

Chromosome* Chromosome::clone(const Chromosome& other)
{
    Type type = other.type();
    int ncapacitors = other.ncapacitors();
    int nresistors  = other.nresistors();
    Chromosome *c = Chromosome::create(type, ncapacitors, nresistors, false);

    for (int i = 0; i < other.size(); ++i) {
        (*c)[i] = other[i];
    }
    return c;
}


void Chromosome::canonicalize()
{
    int ncap = ncapacitors();
    int nres = nresistors();
    int len = size();
    int ilen = isize();
    int base = imax() + 1;
    int node = 0;
    QMap<int, int> map;
    QVector<int> nodes(m_IE);

    for (int i = 0; i < ilen; ++i) {
        if (!map.contains(nodes.at(i))) {
            map.insert(nodes.at(i), node++);
        }
    }

    for (int i = 0; i < len; ++i) {
        if (i < ilen) {
            m_IE[i] = map.value(nodes.at(i));
        } else {
            m_IE[i] = map.value(nodes.at(nodes.at(i)));
        }
    }


    for(int i = 2; i < ilen; i+=2) {
        if(m_IE.at(i) < m_IE.at(i-1)) {
            int temp = m_IE.at(i);
            m_IE[i] = m_IE.at(i-1);
            m_IE[i-1] = temp;
        }
    }

    for (int i = 1; i < ncap; ++i) {
        for (int j = 2; j < ncap*2; j+=2) {
            if (m_IE.at(j-1)*base + m_IE.at(j) > m_IE.at(j+1)*base + m_IE.at(j+2)) {
                int temp = m_IE.at(j+2);
                m_IE[j+2] = m_IE.at(j);
                m_IE[j] = temp;

                temp = m_IE.at(j+1);
                m_IE[j+1] = m_IE.at(j-1);
                m_IE[j-1] = temp;
            }
        }
    }
    for (int i = 1; i < nres; ++i) {
        for (int j = ncap*2+2; j < (ncap+nres)*2; j+=2) {
            if (m_IE.at(j-1)*base + m_IE.at(j) > m_IE.at(j+1)*base + m_IE.at(j+2)) {
                int temp = m_IE.at(j+2);
                m_IE[j+2] = m_IE.at(j);
                m_IE[j] = temp;

                temp = m_IE.at(j+1);
                m_IE[j+1] = m_IE.at(j-1);
                m_IE[j-1] = temp;
            }
        }
    }

    for (int i = isize(); i < size(); ++i) {
        m_IE[i] = m_IE.indexOf(m_IE.at(i));
    }
}
