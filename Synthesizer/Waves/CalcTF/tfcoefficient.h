#ifndef TFCOEFFICIENT_H
#define TFCOEFFICIENT_H


class TfCoeff
{
public:
    TfCoeff();
    double upperV() const;
    void setUpperV(double upperV);

    double lowerV() const;
    void setLowerV(double lowerV);

private:
    double m_upperV;
    double m_lowerV;
};

#endif // TFCOEFFICIENT_H
