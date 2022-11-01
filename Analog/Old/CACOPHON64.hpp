#pragma once

// CACOPHON-64
// 64 Bandlimited Analog Waveform Generator VCO
// 64 Moog VCF State Variablized FilterBank

struct CacophonVCO64 : public OscillatorProcessor
{
    PolyBLEP *wave[64];
    FADSR    *env[64];
    FunctionLFO * lfo[64];

    float     semi[64];
    float     tune[64];
    float     fine[64];
    double    A[64];
    double    F[64];
    int       W[64];
    bool      E[64];
    double    FM[64][64];
    double    PM[64][64];

    CacophonVCO64() : OscillatorProcessor()
    {
        for(size_t i = 0; i < 64; i++)
        {
            wave[i] = new PolyBLEP(sampleRate);
        }
    }
    ~Cacophon64()
    {
        for(size_t i = 0; i < 64; i++)
        {
            delete wave[i];
    }
    void setFrequency(size_t idx, double f) {
        F[idx] = f;
    }
    void setAmplitude(size_t idx, double a) {
        A[idx] = a;
    }
    void setEnable(size_t idx, bool s) {
        E[idx] = s;
    }
    void setWaveform(size_t idx, int w) {
        W[idx] = w;
    }
    void setSemi(size_t idx, float s) {
        semi[idx] = s;
    }
    void setTune(size_t idx, float s) {
        tune[idx] = s;
    }
    void setFine(size_t idx, float s) {
        fine[idx] = s;
    }
    double Tick(double I=1, double A=1, double X=0, double Y=0)
    {
        double out = 0;
        for(size_t i = 0; i < 64; i++)
        {

        }
    }
};