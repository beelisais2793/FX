#include "tfcoefficient.h"

TfCoeff::TfCoeff()
{
    setUpperV(0.0);
    setLowerV(0.0);
}

double TfCoeff::upperV() const
{
    return m_upperV;
}

void TfCoeff::setUpperV(double upperV)
{
    //if (upperV < this->lowerV()) return;
    m_upperV = upperV;
}

double TfCoeff::lowerV() const
{
    return m_lowerV;
}

void TfCoeff::setLowerV(double lowerV)
{
    //if (lowerV > this->upperV()) return;
    m_lowerV = lowerV;
}
