#include "Envelope.hpp"

class ADSR : public Envelope
{
    Sample sample = 0;
    Sample down = 0;
    double phase = -1;
    bool gate = false;

    double A = 0.02; // s
    double D = 0.1; // s
    double S = 1.0; // (0-1)
    double R = 0.1; // s

    double AC = 0.5;
    double DC = 0.5;
    double RC = 0.5;

public:


    ADSR(double a, double d, double s, double r) : A(a), D(d), S(s), R(r) {};

    ADSR& Attack(double);
    ADSR& Decay(double);
    ADSR& Sustain(double);
    ADSR& Release(double);

    Sample Apply(Sample) override;

    Sample NextSample() override;
    Sample GetSample() override;
    
    void Trigger() override;
    void Gate(bool) override;
};