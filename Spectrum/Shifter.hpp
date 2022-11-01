#pragma once


#include <complex>
#include <vector>
#include <memory>

namespace FX::SHIFTER
{
    typedef std::complex<float> Cfloat;

    class STFT
    {
    public:
        STFT() {}
        ~STFT() {}

        void init(int);
        void step(float*, int, Cfloat*);

    private:
        int windowlen;

        std::vector<float> sampleframe;
        std::vector<float> hannwindow;
        std::vector<Cfloat> fftinput;

        std::unique_ptr<juce::dsp::FFT> fft;
    };

    class ISTFT
    {
    public:
        ISTFT() {}
        ~ISTFT() {}

        void init(int);
        void step(Cfloat*, float*, int);

    private:
        int windowlen;

        std::vector<float> sampleframe;
        std::vector<float> hannwindow;
        std::vector<Cfloat> fftoutput;

        float hannsum;

        std::unique_ptr<juce::dsp::FFT> fft;
    };


    class SHIFTER
    {
    public:
        SHIFTER();
        ~SHIFTER() {}

        void init(int);
        void step(float*, int, float);

    private:
        STFT stft;
        ISTFT istft;

        int windowlength;

        std::vector<Cfloat> fftdata;
        std::vector<float> fftmag;
        std::vector<float> fftphase;
        std::vector<float> oldphase;
        std::vector<float> modphase;
        std::vector<float> deltaphase;
        bool firsttime;
    };



    void STFT::init(int order)
    {
        windowlen = int(pow(2.0f, order));
        hannwindow.assign(windowlen, 0.0f);
        sampleframe.assign(windowlen, 0.0f);
        fftinput.assign(windowlen, 0.0f);
        float pi = 3.14159f;
        for (int n = 0; n < windowlen; n++)
            hannwindow[n] = sqrt(0.5f * (1.0f - cosf(2.0f * pi * float(n) / float(windowlen))));
        fft = std::make_unique<juce::dsp::FFT>(order);
    }

    void STFT::step(float *samples, int numsamples, Cfloat *fftoutput)
    {
        // Shift samples into frame
        for (int n = numsamples; n < windowlen; n++)
            sampleframe[n - numsamples] = sampleframe[n];
        for (int n = 0; n < numsamples; n++)
            sampleframe[windowlen - numsamples + n] = samples[n];
        // Apply window
        for (int n = 0; n < windowlen; n++)
            fftinput[n] = sampleframe[n] * hannwindow[n];
        // Compute FFT
        fft->perform(fftinput.data(), fftoutput, false);
    }

    void ISTFT::init(int order)
    {
        windowlen = int(pow(2.0f, order));
        hannwindow.assign(windowlen, 0.0f);
        sampleframe.assign(windowlen, 0.0f);
        fftoutput.assign(windowlen, 0.0f);
        float pi = 3.14159f;
        hannsum = 0.0f;
        for (int n = 0; n < windowlen; n++)
        {
            hannwindow[n] = sqrt(0.5f * (1.0f - cosf(2.0f * pi * float(n) / float(windowlen))));
            hannsum += hannwindow[n];
        }
        fft = std::make_unique<juce::dsp::FFT>(order);
    }

    void ISTFT::step(Cfloat *fftinput, float *samples, int numsamples)
    {
        // Enforce conjugate symmetry
        int m = windowlen - 1;
        int N = windowlen / 2;
        for (int n = 1; n < N; n++)
            fftinput[m--] = std::conj(fftinput[n]);
        fftinput[N] = std::real(fftinput[N]);
        // Compute inverse FFT
        fft->perform(fftinput, fftoutput.data(), true);
        // Add windowed output to sample frame
        for (int n = 0; n < windowlen; n++)
            sampleframe[n] += fftoutput[n].real() * hannwindow[n];
        // Get output samples
        for (int n = 0; n < numsamples; n++)
            samples[n] = sampleframe[n] * float(numsamples) / hannsum;
        // Shift sample frame
        for (int n = numsamples; n < windowlen; n++)
            sampleframe[n - numsamples] = sampleframe[n];
        for (int n = windowlen - numsamples; n < windowlen; n++)
            sampleframe[n] = 0.0f;
    }

    void PVresample(float *x, int Nx, float *y, int Ny)
    {
        float ratio = float(Nx - 1) / float(Ny - 1);
        y[0] = x[0];
        y[Ny - 1] = x[Nx - 1];
        for (int n = 1; n < Ny - 1; n++)
        {
            float xn = ratio * float(n);
            int n1 = int(floor(xn));
            y[n] = x[n1] + (xn - floor(xn)) * (x[n1 + 1] - x[n1]);
        }
    }

    SHIFTER::SHIFTER()
    {
        windowlength = 1;
        firsttime = true;
    }

    void SHIFTER::init(int order)
    {
        stft.init(order);
        istft.init(order);
        windowlength = pow(2.0f, order);
        fftdata.assign(windowlength, 0.0f);
        fftmag.assign(windowlength, 0.0f);
        fftphase.assign(windowlength, 0.0f);
        oldphase.assign(windowlength, 0.0f);
        modphase.assign(windowlength, 0.0f);
        deltaphase.assign(windowlength, 0.0f);
        firsttime = true;
    }

    void SHIFTER::step(float *samples, int numsamples, float hopratio)
    {
        // Calculate STFT
        stft.step(samples, numsamples, fftdata.data());
        // Do strange and wonderful things in the frequency domain
        float pi = 3.14159;
        for (int n = 0; n < windowlength; n++)
        {
            // Separate magnitude and phase, saving previous phase
            oldphase[n] = fftphase[n];
            fftmag[n] = std::abs(fftdata[n]);
            fftphase[n] = std::arg(fftdata[n]);
            // Unwrap and modify phase
            deltaphase[n] = fftphase[n] - oldphase[n] - 2.0f * pi * float(numsamples) * float(n) / float(windowlength);
            deltaphase[n] -= 2.0f * pi * round(deltaphase[n] / (2.0f * pi));
            deltaphase[n] = (deltaphase[n] + 2.0f * pi * float(numsamples) * float(n) / float(windowlength)) * hopratio;
            if (firsttime)
            {
                modphase[n] = fftphase[n];
            }
            else
                modphase[n] += deltaphase[n];
            // Convert back to rectangular
            fftdata[n] = std::polar(fftmag[n], modphase[n]);
        }
        firsttime = false;
        // Calculate ISTFT using modified block size
        int synthlength = int(round(hopratio * float(numsamples)));
        std::vector<float> synthsamples;
        synthsamples.assign(synthlength, 0.0f);
        istft.step(fftdata.data(), synthsamples.data(), synthlength);
        // Resample to original block size
        PVresample(synthsamples.data(), synthlength, samples, numsamples);
    }
}