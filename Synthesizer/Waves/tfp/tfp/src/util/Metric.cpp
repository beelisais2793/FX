#include "tfp/util/Metric.hpp"
#include "tfp/util/String.hpp"

#include <cstring>

namespace tfp {

const char* Metric::metricTable[] = {
    "a",
    "f",
    "p",
    "n",
    "u",
    "m",
    "",
    "k",
    "M",
    "G",
    "T",
    "P",
    "E",
    // exceptions start here
    "dB",
    "t",
    "s",
    "min",
    "h",
    "d"
};


// ----------------------------------------------------------------------------
Metric::Metric() :
    metric_(UNITY)
{
}

// ----------------------------------------------------------------------------
Metric::Metric(Metric_e metric) : metric_(metric)
{
}

// ----------------------------------------------------------------------------
Metric Metric::fromString(const char* str)
{
    unsigned len = strlen(str);
    for(Metric m = Metric::ATTO;; ++m)
    {
        unsigned offset;
        if((offset = str::strcmpoff(metricTable[m.toUInt()], str)) > 0)
            if(len == strlen(metricTable[m.toUInt()]))
                return m;

        if(m == Metric::DAYS)
            break;
    }

    return Metric();
}

// ----------------------------------------------------------------------------
unsigned int Metric::metricStringOffset(const char* str)
{
    unsigned ret = 0;
    unsigned len = strlen(str);
    for(Metric m = Metric::ATTO;; ++m)
    {
        unsigned offset;
        if((offset = str::strcmpoff(metricTable[m.toUInt()], str)) > 0)
        {
            // If length of string matches with that in the table, we found it
            if(len == strlen(metricTable[m.toUInt()]))
                return offset;
            // Store this offset, there could be a potentially larger one.
            if(offset > ret)
                ret = offset;
        }

        if(m == Metric::DAYS)
            break;
    }

    return ret;
}

// ----------------------------------------------------------------------------
Metric& Metric::operator=(unsigned rhs)
{
    metric_ = static_cast<Metric_e>(rhs);
    if(metric_ > DAYS)
        metric_ = DAYS;
    return *this;
}

// ----------------------------------------------------------------------------
Metric& Metric::operator+=(unsigned rhs)
{
    unsigned iMetric = toUInt();
    iMetric += rhs;
    if(iMetric > DAYS)
        iMetric = DAYS;
    metric_ = static_cast<Metric_e>(iMetric);
    return *this;
}

// ----------------------------------------------------------------------------
Metric& Metric::operator-=(unsigned rhs)
{
    unsigned iMetric = toUInt();
    iMetric -= rhs;
    if(iMetric > DAYS)
        iMetric = ATTO;
    metric_ = static_cast<Metric_e>(iMetric);
    return *this;
}

// ----------------------------------------------------------------------------
Metric& Metric::operator++()
{
    unsigned iMetric = toUInt();
    if(iMetric < DAYS)
        iMetric++;
    metric_ = static_cast<Metric_e>(iMetric);
    return *this;
}

// ----------------------------------------------------------------------------
Metric Metric::operator++(int)
{
    Metric tmp(*this);
    operator++();
    return tmp;
}

// ----------------------------------------------------------------------------
Metric& Metric::operator--()
{
    unsigned iMetric = toUInt();
    if(iMetric > ATTO)
        iMetric--;
    metric_ = static_cast<Metric_e>(iMetric);
    return *this;
}

// ----------------------------------------------------------------------------
Metric Metric::operator--(int)
{
    Metric tmp(*this);
    operator--();
    return tmp;
}

// ----------------------------------------------------------------------------
const char* Metric::toString() const
{
    return metricTable[metric_];
}

// ----------------------------------------------------------------------------
unsigned Metric::toUInt() const
{
    return static_cast<unsigned>(metric_);
}

} // namespace tfp
