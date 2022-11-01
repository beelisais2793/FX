#pragma once

class TSClipper
{
public:
    
    TSClipper();
    
    ~TSClipper();
    
    void prepare(DspFloatType newFs);

    DspFloatType processSample(DspFloatType Vi);
    
    void setKnob(DspFloatType newDrive);
    
private:
    
    const DspFloatType eta = 1.f; // Change for non-ideal diode
    const DspFloatType Is = 1e-15;
    const DspFloatType Vt = 26e-3;
    
    DspFloatType Fs = 48000.f;
    DspFloatType Ts = 1.f/Fs;
    
    DspFloatType C1 = 47e-9;
    DspFloatType R1 = Ts/(2.f*C1);
    DspFloatType C2 = 51e-12;
    DspFloatType R2 = Ts/(2.*C2);
    DspFloatType drivePot = 1.f;
    DspFloatType P1 = drivePot*500e3;
    DspFloatType R3 = 51000.f + P1;
    DspFloatType R4 = 4700.f;

    // Combined Resistances
    DspFloatType G1 = (1.f + R4/R1);
    DspFloatType G4 = (1.f + R1/R4);
    
    // States
    DspFloatType x1 = 0.f;
    DspFloatType x2 = 0.f;
    DspFloatType Vd = 0.f;
    
    DspFloatType thr = 0.00000000001f;
    
    void updateCoefficients();
};

TSClipper::TSClipper(){}

TSClipper::~TSClipper(){}

void TSClipper::prepare(DspFloatType newFs)
{
    if (Fs != newFs)
    {
        Fs = newFs;
        updateCoefficients();
    }
}

DspFloatType TSClipper::processSample(DspFloatType Vi)
{
    DspFloatType p = -Vi/(G4*R4) + R1/(G4*R4)*x1 - x2;
    int iter = 1;
    DspFloatType b = 1.f;
    DspFloatType fd = p + Vd/R2 + Vd/R3 + 2.f*Is * sinh(Vd/(eta*Vt));
    
    while (iter < 50 && abs(fd) > thr)
    {
        DspFloatType den = 2.f*Is/(eta*Vt) * cosh(Vd/(eta*Vt)) + 1.f/R2 + 1.f/R3;
        DspFloatType Vnew = Vd - b*fd/den;
        DspFloatType fn = p + Vnew/R2 + Vnew/R3 + 2.f*Is * sinh(Vnew/(eta*Vt));
        
        if (abs(fn) < abs(fd))
        {
            Vd = Vnew;
            b = 1.f;
        }
        else
            b *= 0.5f;
    
        fd = p + Vd/R2 + Vd/R3 + 2.f*Is * sinh(Vd/(eta*Vt));
        iter++;
    }
    
    DspFloatType Vo = Vd + Vi;
    x1 = (2.f/R1)*(Vi/G1 + x1*R4/G1) - x1;
    x2 = (2.f/R2)*(Vd) - x2;
    
    return Vo;
}

void TSClipper::setKnob(DspFloatType newDrive)
{
    if (drivePot != newDrive)
    {
        drivePot = newDrive;
        updateCoefficients();
    }
}

void TSClipper::updateCoefficients()
{
    Ts = 1.f/Fs;
    
    R1 = Ts/(2.f*C1);
    R2 = Ts/(2.f*C2);
    P1 = drivePot * 500e3;
    R3 = 51000.f + P1;
    
    // Combined Resistances
    G1 = (1.f + R4/R1);
    G4 = (1.f + R1/R4);
}