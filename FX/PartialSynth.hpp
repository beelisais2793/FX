// https://github.com/njazz/partialSynth
#include <stdio.h>

class PartialParameter {
    float _value = 0;
    float _previous = 0;
    float _smooth = 0.998;
    float _nsmooth = .002;

public:
    inline
    void process()
    {
        _previous = _previous * _smooth + _value * _nsmooth;
    }

    inline
    void process(size_t s)
    {
        while (s--) {
            process();
        }
    }

    inline
    void setValue(float v)
    {
        _value = v;
        process();
    }
    
    inline
    float value() { return _previous; }
    
    inline float* valuePtr() {return &_previous;}

    inline
    void setSmooth(float v)
    {
        _smooth = v;
        _nsmooth = 1 - v;
    }
    
    inline
    float smooth() { return _smooth; }
};

class BasePartial {
protected:
    bool _busy = false;
    float _sampleRate = 44100;
    
    PartialParameter _param[3];
    
    template<int I>
    inline
    PartialParameter& par()
    {
        return _param[I];
    };

public:
    void process(size_t s) {}
    
    template <int I>
    inline
    void set(float f)
    {
        par<I>().setValue(f);
    };
    
    template <int I>
    inline
    float value()
    {
        return par<I>().value();
    };

    template <int I>
    inline
    void setSmooth(float f)
    {
        par<I>().setSmooth(f);
    };

    bool busy() { return _busy; }
    void setBusy(bool b) { _busy = b; }
    
    void setSampleRate(float sr){_sampleRate = sr;}
};


class SineTable {
    static float _v[4096];

public:
    SineTable();
    float operator()(float x);
};

// ---

class CosTable {
    static float _v[4096];
    
public:
    CosTable();
    float operator()(float x);
};

class ResPartial : public BasePartial {

private:
    float fRec0[2];
    float fRec2[2];
    float fRec3[2];
    float fRec1[2];
    int fSamplingFreq = 44100;
    float fConst0 = (6.2831855f / fmin(1.92e+05f, fmax(1.0f, (float)fSamplingFreq)));
    //
    SineTable _sin;
    CosTable _cos;

    float* _freq;
    float* _decay;
    volatile float* _gain;

    float _cTemp1 = 0;
    float _cTemp2 = 0;
    float _cTemp3 = 0;

    float _dTemp2 = 0;
    float _dTemp3 = 0;

    inline void compute(const float** input, float** output) //int count,
    {

        const float* input3 = input[0];
        float* output0 = output[0];

        {

            fRec0[0] = ((_dTemp2 * fRec1[1]) + (_dTemp3 * fRec0[1]));
            fRec2[0] = ((_dTemp2 * fRec3[1]) + (_dTemp3 * fRec2[1]));
            fRec3[0] = ((-_cTemp2 * fRec2[1]) + ((float)input3[0] + (_cTemp3 * fRec3[1])));
            fRec1[0] = ((_cTemp3 * fRec1[1]) + (fRec3[0] + (-_cTemp2 * fRec0[1])));

            output0[0] += (*_gain * fRec0[0]);

            // post processing
            fRec0[1] = fRec0[0];
            fRec1[1] = fRec1[0];
            fRec2[1] = fRec2[0];
            fRec3[1] = fRec3[0];
        }
    }

public:
    ResPartial()
    {
        reset();

        par<pGain>().setSmooth(.9997);
        par<pFreq>().setSmooth(0);
        par<pDecay>().setSmooth(0.);

        _busy = false;

        _freq = par<pFreq>().valuePtr(); //input[0];
        _decay = par<pDecay>().valuePtr(); //input[1];
        _gain = par<pGain>().valuePtr(); //input[2];
    }

    void processParams(size_t s)
    {
        while (s--) {
            par<pFreq>().process();
            par<pDecay>().process();
            par<pGain>().process();
        }
    }

    void process(size_t s, const float* in_buffer, float* out_buffer)
    {
        const float* b_in = in_buffer;
        float* b_out = out_buffer;

        int size = (int)s;
        while (size--) {
            par<pGain>().process();

            compute(&b_in, &b_out);
            b_in++;
            b_out++;
        }

        if (par<pGain>().value() < 0.000001)
            if (gate) {
                reset();
                _busy = false;
                gate = false;
            }
    }

    void updateFreq()
    {
        _cTemp1 = (fConst0 * (float)*_freq); //input0);
        _cTemp2 = _sin(_cTemp1);
        _cTemp3 = _cos(_cTemp1);

        _dTemp2 = *_decay * _cTemp2;
        _dTemp3 = *_decay * _cTemp3;
    }

    void reset()
    {
        for (int i = 0; i < 2; i++) {
            fRec0[i] = 0;
            fRec1[i] = 0;
            fRec2[i] = 0;
            fRec3[i] = 0;
        }
    }

    bool gate;

    static const int pFreq = 0;
    static const int pDecay = 1;
    static const int pGain = 2;
};

class SinePartial : public BasePartial {

    unsigned long _counter = 0;

    //inline
    float sinOsc(float f)
    {
        float ret = sinf(float(_counter) / (_sampleRate / f) * 3.1415 * 2);

        _counter++;

        return ret;
    }

public:
    SinePartial()
    {
        par<pFreq>().setSmooth(0);
    }

    void process(size_t s, float* buffer)
    {
        float* b = buffer;
        while (s--) {
            par<pFreq>().process();
            par<pAmp>().process();

            *b += sinOsc(par<pFreq>().value()) * par<pAmp>().value();
            b++;

            if (par<pAmp>().value() < 0.000001)
                _busy = false;
        }
    }

    static const int pFreq = 0;
    static const int pAmp = 1;
};


static const int maxNumberOfPartials = 4096;

typedef struct SinePartialData {
    float* freq = 0;
    float* amp = 0;
    size_t size = 0;

    explicit SinePartialData(size_t s)
    {
        freq = new float[s];
        amp = new float[s];
        size = s;
    }
    SinePartialData(SinePartialData& d)
    {
        freq = new float[d.size];
        amp = new float[d.size];
        size = d.size;

        memcpy(freq, d.freq, size);
        memcpy(amp, d.amp, size);

    }

    ~SinePartialData()
    {
        if (freq)
            delete[] freq;
        if (amp)
            delete[] amp;
    }

} SinePartialData;

class SineSynth {
    std::vector<SinePartial*> partials;

    const std::vector<int> freePartialIndices()
    {
        std::vector<int> ret;
        for (int i = 0; i < maxNumberOfPartials; i++) {
            if (!partials[i]->busy())
                ret.push_back(i);
        }
        return ret;
    }

    const std::vector<SinePartial*> activePartials()
    {
        std::vector<SinePartial*> ret;
        for (int i = 0; i < maxNumberOfPartials; i++) {
            if (partials[i]->busy())
                ret.push_back(partials[i]);
        }
        return ret;
    }

public:
    SineSynth()
    {
        for (int i = 0; i < maxNumberOfPartials; i++)
            partials.push_back(new SinePartial);
    }
    void setData(SinePartialData& data)
    {
        muteActivePartials();

        std::vector<int> freeIdx = freePartialIndices();

        int fi = (int)freeIdx.size();
        int s = (data.size < fi) ? (int)data.size : fi;

        for (int i = 0; i < s; i++) {
            int idx = freeIdx[i];
            if (idx > 1024)
                continue;
            partials[idx]->set<SinePartial::pAmp>(data.amp[i]);
            partials[idx]->set<SinePartial::pFreq>(data.freq[i]);
            partials[idx]->setBusy(true);
        }
    };

    void muteActivePartials()
    {
        for (int i = 0; i < maxNumberOfPartials; i++) {
            if (partials[i]->busy()) {
                partials[i]->set<SinePartial::pAmp>(0);
            }
        }
    }

    void process(float* buffer, size_t s)
    {

        for (int i = 0; i < s; i++)
            buffer[i] = 0.;

        for (SinePartial* sp : activePartials())
            sp->process(s, buffer);
    };
};

float SineTable::_v[4096];
float CosTable::_v[4096];

SineTable::SineTable()
{
    for (int i = 0; i < 4096; i++) {
        _v[i] = sinf(float(i) / 4096 * 3.1415926 * 2);
    }
}

float SineTable::operator()(float x)
{
    int idx = x / 3.1415926 * 2048;
    idx = (idx > 0) ? (idx % 4096) : 4096 - (-idx % 4096);
    return _v[idx];
};

CosTable::CosTable()
{
    for (int i = 0; i < 4096; i++) {
        _v[i] = cosf(float(i) / 4096 * 3.1415926 * 2);
    }
}

float CosTable::operator()(float x)
{
    int idx = x / 3.1415926 * 2048;
    idx = (idx > 0) ? (idx % 4096) : 4096 - (-idx % 4096);
    return _v[idx];
};
