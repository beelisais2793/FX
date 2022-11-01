#pragma once

#include "RackFX.hpp"

namespace FX::RackFX
{
    #define MAX_FRAME_LENGTH 2048
    class PitchShifter
    {
    public:
        PitchShifter (long fftFrameSize, long osamp, DspFloatType sampleRate);
        ~PitchShifter ();
        void smbPitchShift (DspFloatType pitchShift, long numSampsToProcess,
                            long fftFrameSize, long osamp, DspFloatType sampleRate,
                            DspFloatType *indata, DspFloatType *outdata);
        void smbFft (DspFloatType *fftBuffer, long fftFrameSize, long sign);
        double smbAtan2 (double x, double y);
        DspFloatType ratio;
    private:
        void makeWindow(long fftFrameSize);
        DspFloatType gInFIFO[MAX_FRAME_LENGTH];
        DspFloatType gOutFIFO[MAX_FRAME_LENGTH];
        DspFloatType gFFTworksp[2 * MAX_FRAME_LENGTH];
        DspFloatType gLastPhase[MAX_FRAME_LENGTH / 2 + 1];
        DspFloatType gSumPhase[MAX_FRAME_LENGTH / 2 + 1];
        DspFloatType gOutputAccum[2 * MAX_FRAME_LENGTH];
        DspFloatType gAnaFreq[MAX_FRAME_LENGTH];
        DspFloatType gAnaMagn[MAX_FRAME_LENGTH];
        DspFloatType gSynFreq[MAX_FRAME_LENGTH];
        DspFloatType gSynMagn[MAX_FRAME_LENGTH];
        double window[MAX_FRAME_LENGTH];
        double dfftFrameSize, coef_dfftFrameSize, dpi_coef;
        double magn, phase, tmp, real, imag;
        double freqPerBin, expct, coefPB, coef_dpi, coef_mpi;
        long k, qpd, index, inFifoLatency, stepSize, fftFrameSize2, gRover, FS_osamp;

        //FFTW variables
        fftw_complex fftw_in[MAX_FRAME_LENGTH], fftw_out[MAX_FRAME_LENGTH];
        fftw_plan ftPlanForward, ftPlanInverse;
    };
}