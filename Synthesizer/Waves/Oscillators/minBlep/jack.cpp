#include "jack.hpp"
#include <cstdio>
#include <string>
#include <ctime>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sndfile.h>
#include <iostream>

TFraction TEnvelope::Step(int samples)
{
    SampleCounter += samples;

    switch (State) {
    case IDLE:
        SampleCounter = 0;
        Value = 0;
        State = ATTACK;
        // Fall-through
    case ATTACK:
        if (SampleCounter < A) {
            Value = float(SampleCounter) / A;
        }
        else {
            SampleCounter = 0;
            Value = 1;
            State = DECAY;
        }
        break;
    case DECAY:
        if (SampleCounter < D) {
            Value = 1 - (1 - S) * float(SampleCounter) / D;
        }
        else {
            SampleCounter = 0;
            Value = S;
            State = SUSTAIN;
        }
        break;
    case SUSTAIN:
        break;
    case RELEASE:
        if (SampleCounter < R) {
            Value = ReleaseValue - ReleaseValue * float(SampleCounter) / R;
        }
        else {
            Value = 0;
            State = FINISHED;
        }
        break;
    case FINISHED:
        break;
    }
    return Value;
}

void TEnvelope::Release(TUnsigned7 velocity)
{
    SampleCounter = 0;
    ReleaseValue = Value;
    State = RELEASE;
}

struct TNoteData
{
    TUnsigned7 Note;
    TUnsigned7 Velocity;
};


TTimer::TTimer(const std::string& name)
        : Name(name), StartTime(), Nanoseconds(0), MaxNs(0), Count(0)
{
}

TTimer::~TTimer()
{
    Log();
}

void TTimer::Log() const
{
    float max = MaxNs / 1000000.0;
    float average = Nanoseconds / (Count > 0 ? Count : 1) / 1000000.0;

    printf("[Timer] %s: %d invocations, total %.3f ms, average %.3f ms, max %.3f ms\n",
            Name.c_str(), Count, Nanoseconds / 1000000.0, average, max);
}

void TTimer::ReportOpsPerSecond(unsigned ops) const
{
    double mflops = double(1000LLU * ops * Count) / double(Nanoseconds);
    printf("[Timer] %s: %.3f MFLOPS\n", Name.c_str(), mflops);
}


TSampleLoader::TSampleLoader(const std::string& filename)
        : Buffer(0, 0), Data(), SampleRate(0)
{
    SF_INFO info;
    memset(&info, 0, sizeof(info));
    SNDFILE* sf = sf_open(filename.c_str(), SFM_READ, &info);
    if (sf != 0) {
        std::cerr << "Opened sample file: " << info.frames << " frames, " << info.samplerate
                << " Hz, " << info.channels << " channels." << std::endl;
        int n = info.frames * info.channels;
        // FIXME: We're reading all channels interleaved here...
        Data.resize(n);
        sf_read_float(sf, Data.data(), n);
        Buffer = TSampleBuffer(Data.data(), n);
        SampleRate = info.samplerate;
        sf_close(sf);
    }
    else {
        std::cerr << "Failed to open sample file " << filename << std::endl;
    }
}



/*
 * Measurements at 880Hz PW=50%, sample rate 44100Hz.
 *   --> 50.1 samples per period.
 * nzc=16, os=32 (--> 32 samples blep length):
 *    fundamental at -34dB, harmonics at -44 -48 -51 -53 -54 -57
 *    worst aliasing at -98dB, first peak at 538Hz.
 * nzc= 8, os=32 (--> 16 samples blep length):
 *    fundamental at -34dB
 *    worst aliasing at -98dB, first peak at 538Hz.
 * nzc=16, os=16 (--> 32 samples blep length):
 *    fundamental at -34dB
 *    worst aliasing at -91dB, first peak at 710Hz.
 * nzc= 8, os=64 (--> 16 samples blep length):
 *    fundamental at -34dB
 *    worst aliasing at -103dB, first peak at 237Hz.
 *    Double peaks above 16.5kHz
 * nzc=16, os=64 (--> 32 samples blep length):
 *    fundamental at -34dB
 *    worst aliasing at -104dB, first peak at 237Hz.
 *    Double peaks above 19kHz
 * nzc= 8, os=128 (--> 16 samples blep length):
 *    fundamental at -34dB
 *    horrible double peaks above 15kHz
 */


TMinBlepPulseOscillator::TMinBlepPulseOscillator()
        : BufferPos(0), Target(0), Pw(0), NextPw(0)
{
    PhaseAccumulator = 0.99999999;
    std::fill(Buffer, Buffer + BufferLen, 0);
}

void TMinBlepPulseOscillator::Process(TSampleBuffer& in, TSampleBuffer& out,
        TSampleBuffer& syncin, TSampleBuffer& syncout)
{
    const float A = TGlobal::OscAmplitude;
    for (TSample& outs : out) {
        float pace = Hz / TGlobal::SampleRate;
        assert(pace > 0);
        if (pace > 0.99) pace = 0.99;

        double lastpos = PhaseAccumulator;
        PhaseAccumulator += pace;
        if (PhaseAccumulator >= 1) {
            PhaseAccumulator -= 1;
            Pw = NextPw;
        }
        const float pw = 0.5 + 0.5 * Pw;
        assert(PhaseAccumulator >= 0);
        assert(PhaseAccumulator <= 1);

        // If Scanpos just went past 0, we need to place a positive
        // edge. If it just went past pw, we need to place a negative
        // edge. The step is constructed from minblep::length samples
        // chosen from minblep::table.

        if ((lastpos < pw && PhaseAccumulator >= pw)
                || (lastpos < pw && lastpos > PhaseAccumulator)) {
            int offset = (PhaseAccumulator - pw) / pace * minblep::overSampling;
            if (PhaseAccumulator < pw) {
                offset = ((1 + PhaseAccumulator) - pw) / pace * minblep::overSampling;
            }
            for (int i = 0; i < BufferLen; i++) {
                Buffer[(BufferPos + i) % BufferLen] -= minblep::table[i
                        * minblep::overSampling + offset];
            }
            Target = 0;
        }
        if (lastpos > PhaseAccumulator) {
            const int offset = PhaseAccumulator / pace * minblep::overSampling;
            for (int i = 0; i < BufferLen; i++) {
                Buffer[(BufferPos + i) % BufferLen] += minblep::table[i
                        * minblep::overSampling + offset];
            }
            Target = 1;
        }

        outs += A * (Buffer[BufferPos % BufferLen] - pw);
        Buffer[BufferPos % BufferLen] = Target;
        BufferPos++;
    }
    syncout.Clear();
}

TMinBlepSawOscillator::TMinBlepSawOscillator()
        : BufferPos(0)
{
    PhaseAccumulator = 0.5f;
    std::fill(Buffer, Buffer + BufferLen, 1.0f);
}

/*
 *  |    |    |
 *  |   /|   /|
 *  |  / |  / |
 *  | /  | /  | /
 *  |/   |/   |/
 *
 *  ^    ^    ^  Place negative minBLEPs here.
 */
void TMinBlepSawOscillator::Process(TSampleBuffer& in, TSampleBuffer& out,
        TSampleBuffer& syncin, TSampleBuffer& syncout)
{
    const float A = TGlobal::OscAmplitude;
    for (size_t sn = 0; sn < out.GetCount(); sn++) {
        float pace = Hz / TGlobal::SampleRate;
        assert(pace > 0);
        if (pace > 0.99) pace = 0.99;

        double lastpos = PhaseAccumulator;
        PhaseAccumulator += pace;
        if (PhaseAccumulator >= 1) {
            PhaseAccumulator -= 1;
        }

        float stepA = 1.0f;
        if (Sync && syncin[sn] > 0) {
            stepA = lastpos; // FIXME: This is not precise
            PhaseAccumulator = syncin[sn] * pace; // FIXME: Is this right?
        }

        assert(PhaseAccumulator >= 0);
        assert(PhaseAccumulator <= 1);

        if (lastpos > PhaseAccumulator) {
            // Wrapped around or synced to beginning of wave. Place a step here.
            syncout[sn] = PhaseAccumulator / pace + 0.0000001;
            const int offset = PhaseAccumulator / pace * minblep::overSampling;
            for (int i = 0; i < BufferLen; i++) {
                Buffer[(BufferPos + i) % BufferLen] += stepA
                        * (-1
                                + minblep::table[i * minblep::overSampling
                                        + offset]);
            }
        }
        else {
            syncout[sn] = 0;
        }

        // Correct for the fact that the BLEP has an average (integral) that is different from a perfect step
        TSample offsetCorrection = (minblep::integral - minblep::length) * pace;

        out[sn] += A
                * (0.5 + PhaseAccumulator - Buffer[BufferPos % BufferLen]
                        + offsetCorrection);
        Buffer[BufferPos % BufferLen] = 1.0f;
        BufferPos++;
    }
}

TButterworthLpFilter::TCache TButterworthLpFilter::Cache;

TButterworthLpFilter::TButterworthLpFilter()
        : Coeffs(), Cutoff(-1)
{
    std::fill(X, X + N + 1, 0);
    std::fill(Y, Y + N + 1, 0);
}

/*
 * FIXME: The cutoff can be changed in "run time", and if that is done
 * slowly the glitches will be minimal. When jumping to a cutoff that
 * is significantly lower, the gain will make a huge positive jump
 * resulting in a pop in the output.
 */
void TButterworthLpFilter::SetCutoff(int cutoff)
{
    float fcutoff = float(cutoff) / TGlobal::NyquistFrequency;
    fcutoff = clamp(fcutoff, 0.01f, 0.95f);
    if (fcutoff == Cutoff) {
        return;
    }
    Cutoff = fcutoff;

    auto cached = Cache.find(cutoff);
    if (cached != Cache.end()) {
        Coeffs = cached->second;
        return;
    }

    double* d = dcof_bwlp(N, Cutoff);
    std::copy(&d[0], &d[N + 2], Coeffs.D);
    free(d);
    d = 0;
    int* c = ccof_bwlp(N);
    std::copy(&c[0], &c[N + 2], Coeffs.C);
    free(c);
    c = 0;
    Coeffs.Gain = sf_bwlp(N, Cutoff);

    Cache[cutoff] = Coeffs;

#if 0
    fprintf(stderr, "Butterworth LP of degree %d, cutoff %.3f*pi, gain compensation %e:\n",
            N, Cutoff, Coeffs.Gain);
    fprintf(stderr, "  D = ");
    for (int i=0; i<=N; i++) {
        fprintf(stderr, "%.3f ", Coeffs.D[i]);
    }
    fprintf(stderr, "\n  C = ");
    for (int i=0; i<=N; i++) {
        fprintf(stderr, "%.3f ", Coeffs.C[i]);
    }
    fprintf(stderr, "\n");
#endif
}

void TButterworthLpFilter::Process(TSampleBuffer& in, TSampleBuffer& out)
{
    std::transform(in.begin(), in.end(), out.begin(),
            [this](const TSample& ins) -> TSample {
                for (int n=N; n>0; n--) {
                    X[n] = X[n-1];
                    Y[n] = Y[n-1];
                }
                X[0] = ins * Coeffs.Gain;
                Y[0] = 0;
                for (int n=0; n<=N; n++) {
                    Y[0] += Coeffs.C[n]*X[n] - Coeffs.D[n]*Y[n];
                }
                return Y[0];
            });
}

TWavetable::TWavetable()
{
}

TWavetable generateSquareTable(unsigned wavelength)
{
    TTimer timer("Generate square table");
    timer.Start();

    TWavetable table;
    table.BaseNote = 8;
    table.Tables = 37;
    table.NotesPerTable = 3;
    table.Length = wavelength;

    table.Data = new TSample[table.Tables * table.Length];

    unsigned nyquist = 44100 / 2;

    for (unsigned t = 0; t < table.Tables; t++) {
        const unsigned f = NOTE2HZ(table.BaseNote + (t + 1) * table.NotesPerTable);

        unsigned harmonics = 1;
        while (f * (harmonics + 2) < nyquist) {
            harmonics += 2;
        }

        //printf("Table %d: up to %u Hz, %u harmonics\n", t, f, harmonics);

        for (unsigned i = 0; i < table.Length; i++) {
            double phase = 2.0 * M_PI * double(i) / table.Length;
            double v = 0.0;

            for (int h = harmonics; h > 0; h -= 2) {
                v += sinf(phase * h) * 1.0 / h;
            }

            table.GetTable(t)[i] = v * TGlobal::OscAmplitude;
        }
    }

    timer.Stop();
    return table;
}

TWavetable generateSineTable(unsigned wavelength)
{
    TTimer timer("Generate sine table");
    timer.Start();

    TWavetable table;
    table.BaseNote = 1;
    table.Tables = 1;
    table.NotesPerTable = 12;
    table.Length = wavelength;

    table.Data = new TSample[table.Tables * table.Length];

    for (unsigned i = 0; i < wavelength; i++) {
        double phase = 2.0 * M_PI * double(i) / wavelength;
        table.GetTable(0)[i] = sin(phase) * TGlobal::OscAmplitude;
    }

    timer.Stop();
    return table;
}

const TWavetable TWavetableOscillator::Table(generateSquareTable(Wavelength));

TWavetableOscillator::TWavetableOscillator(int Note)        
{
    int n = (Note - Table.BaseNote) / Table.NotesPerTable;
    n = clamp(n, 0, (int)Table.Tables - 1);
    Subtable = Table.GetTable(n);
}


TDelayFx::TDelayFx()
: TBaseEffect(),
  WritePos(0),
  LfoPhaseAccumulator(0.0f),
  DelayCrossFadePos(0),
  Distortion(0.0f),
  OldDelay(0),
  Delay(ms_to_samples(500)),
  Feedback(0.75),
  LfoDepth(ms_to_samples(0)),
  LfoSpeed(0.0f),
  LfoChannelPhase(0.6f),
  InsertGain(1.0f),
  Pingpong(0.8f)
{
    std::fill(Buffer[0], Buffer[0] + BufferSize, 0);
    std::fill(Buffer[1], Buffer[1] + BufferSize, 0);
    Filter[0].SetCutoffHz(3000);
    Filter[1].SetCutoffHz(3000);
}

void TDelayFx::Process(TSampleBufferCollection& in, TSampleBufferCollection& out)
{
    // assert stereo...
    assert(in.size() == 2);
    assert(out.size() == 2);

    const int framesize = in[0]->GetCount();

    TSampleBuffer& inl = *in[0];
    TSampleBuffer& inr = *in[1];
    TSampleBuffer& outl = *out[0];
    TSampleBuffer& outr = *out[1];

    // Write delayed samples in the buffer, just like a tape delay.
    // The buffer size does not really matter; the Delay parameter sets the delay in effect.
    // The buffer size just has to be larger than the delay in samples.
    for (int i = 0; i < framesize; i++) {
        LfoPhaseAccumulator += LfoSpeed;
        const TFraction crossFade = float(DelayCrossFadePos) / DelayCrossFadeLength;
        DelayCrossFadePos = std::min(DelayCrossFadePos + 1, DelayCrossFadeLength);
        const float delayl[2] = { OldDelay + LfoDepth * sinf(LfoPhaseAccumulator),
                Delay + LfoDepth * sinf(LfoPhaseAccumulator) };
        const float delayr[2] = { OldDelay + LfoDepth * sinf(LfoPhaseAccumulator + LfoChannelPhase * M_PI),
                Delay + LfoDepth * sinf(LfoPhaseAccumulator + LfoChannelPhase * M_PI) };

        // Pull out interpolated values from the buffer, based on the LFO modulated offset.
        // We add BufferSize to the position so we get a positive number to do modulo with.
        const TSample samplel = mix(linterpolate(Buffer[0], BufferSize, BufferSize + WritePos - delayl[0]),
                linterpolate(Buffer[0], BufferSize, BufferSize + WritePos - delayl[1]), crossFade);
        const TSample sampler = mix(linterpolate(Buffer[1], BufferSize, BufferSize + WritePos - delayr[0]),
                linterpolate(Buffer[1], BufferSize, BufferSize + WritePos - delayr[1]), crossFade);

        TSample vl = InsertGain * (Pingpong * inl[i]) +
                Feedback * ((1 - Pingpong) * samplel + Pingpong * sampler);

        TSample vr = InsertGain * ((1 - Pingpong) * inr[i]) +
                Feedback * ((1 - Pingpong) * sampler + Pingpong * samplel);

        Buffer[0][WritePos] = Distortion.MusicDsp1(Filter[0].ProcessOne(vl));
        Buffer[1][WritePos] = Distortion.MusicDsp1(Filter[1].ProcessOne(vr));

        outl[i] = mix(inl[i], samplel, Mix);
        outr[i] = mix(inr[i], sampler, Mix);

        WritePos = (WritePos + 1) % BufferSize;
    }
}




/*
 *                            COPYRIGHT
 *
 *  liir - Recursive digital filter functions
 *  Copyright (C) 2007 Exstrom Laboratories LLC
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  A copy of the GNU General Public License is available on the internet at:
 *
 *  http://www.gnu.org/copyleft/gpl.html
 *
 *  or you can write to:
 *
 *  The Free Software Foundation, Inc.
 *  675 Mass Ave
 *  Cambridge, MA 02139, USA
 *
 *  You can contact Exstrom Laboratories LLC via Email at:
 *
 *  stefan(AT)exstrom.com
 *
 *  or you can write to:
 *
 *  Exstrom Laboratories LLC
 *  P.O. Box 7651
 *  Longmont, CO 80501, USA
 *
 */



/**********************************************************************
  binomial_mult - multiplies a series of binomials together and returns
  the coefficients of the resulting polynomial.
  
  The multiplication has the following form:
  
  (x+p[0])*(x+p[1])*...*(x+p[n-1])
  The p[i] coefficients are assumed to be complex and are passed to the 
  function as a pointer to an array of doubles of length 2n.
  The resulting polynomial has the following form:
  
  x^n + a[0]*x^n-1 + a[1]*x^n-2 + ... +a[n-2]*x + a[n-1]
  
  The a[i] coefficients can in general be complex but should in most
  cases turn out to be real. The a[i] coefficients are returned by the
  function as a pointer to an array of doubles of length 2n. Storage
  for the array is allocated by the function and should be freed by the
  calling program when no longer needed.
  
  Function arguments:
  
  n  -  The number of binomials to multiply
  p  -  Pointer to an array of doubles where p[2i] (i=0...n-1) is
        assumed to be the real part of the coefficient of the ith binomial
        and p[2i+1] is assumed to be the imaginary part. The overall size
        of the array is then 2n.
*/

double *binomial_mult( int n, double *p )
{
    int i, j;
    double *a;

    a = (double *)calloc( 2 * n, sizeof(double) );
    if( a == NULL ) return( NULL );

    for( i = 0; i < n; ++i )
    {
	for( j = i; j > 0; --j )
	{
	    a[2*j] += p[2*i] * a[2*(j-1)] - p[2*i+1] * a[2*(j-1)+1];
	    a[2*j+1] += p[2*i] * a[2*(j-1)+1] + p[2*i+1] * a[2*(j-1)];
	}
	a[0] += p[2*i];
	a[1] += p[2*i+1];
    }
    return( a );
}


/**********************************************************************
  trinomial_mult - multiplies a series of trinomials together and returns
  the coefficients of the resulting polynomial.
  
  The multiplication has the following form:
  (x^2 + b[0]x + c[0])*(x^2 + b[1]x + c[1])*...*(x^2 + b[n-1]x + c[n-1])
  The b[i] and c[i] coefficients are assumed to be complex and are passed
  to the function as a pointers to arrays of doubles of length 2n. The real
  part of the coefficients are stored in the even numbered elements of the
  array and the imaginary parts are stored in the odd numbered elements.
  The resulting polynomial has the following form:
  
  x^2n + a[0]*x^2n-1 + a[1]*x^2n-2 + ... +a[2n-2]*x + a[2n-1]
  
  The a[i] coefficients can in general be complex but should in most cases
  turn out to be real. The a[i] coefficients are returned by the function as
  a pointer to an array of doubles of length 4n. The real and imaginary
  parts are stored, respectively, in the even and odd elements of the array.
  Storage for the array is allocated by the function and should be freed by
  the calling program when no longer needed.
  
  Function arguments:
  
  n  -  The number of trinomials to multiply
  b  -  Pointer to an array of doubles of length 2n.
  c  -  Pointer to an array of doubles of length 2n.
*/

double *trinomial_mult( int n, double *b, double *c )
{
    int i, j;
    double *a;

    a = (double *)calloc( 4 * n, sizeof(double) );
    if( a == NULL ) return( NULL );

    a[2] = c[0];
    a[3] = c[1];
    a[0] = b[0];
    a[1] = b[1];
  
    for( i = 1; i < n; ++i )
    {
	a[2*(2*i+1)]   += c[2*i]*a[2*(2*i-1)]   - c[2*i+1]*a[2*(2*i-1)+1];
	a[2*(2*i+1)+1] += c[2*i]*a[2*(2*i-1)+1] + c[2*i+1]*a[2*(2*i-1)];

	for( j = 2*i; j > 1; --j )
	{
	    a[2*j]   += b[2*i] * a[2*(j-1)]   - b[2*i+1] * a[2*(j-1)+1] + 
		c[2*i] * a[2*(j-2)]   - c[2*i+1] * a[2*(j-2)+1];
	    a[2*j+1] += b[2*i] * a[2*(j-1)+1] + b[2*i+1] * a[2*(j-1)] +
		c[2*i] * a[2*(j-2)+1] + c[2*i+1] * a[2*(j-2)];
	}

	a[2] += b[2*i] * a[0] - b[2*i+1] * a[1] + c[2*i];
	a[3] += b[2*i] * a[1] + b[2*i+1] * a[0] + c[2*i+1];
	a[0] += b[2*i];
	a[1] += b[2*i+1];
    }

    return( a );
}


/**********************************************************************
  dcof_bwlp - calculates the d coefficients for a butterworth lowpass 
  filter. The coefficients are returned as an array of doubles.
*/

double *dcof_bwlp( int n, double fcf )
{
    int k;            // loop variables
    double theta;     // M_PI * fcf / 2.0
    double st;        // sine of theta
    double ct;        // cosine of theta
    double parg;      // pole angle
    double sparg;     // sine of the pole angle
    double cparg;     // cosine of the pole angle
    double a;         // workspace variable
    double *rcof;     // binomial coefficients
    double *dcof;     // dk coefficients

    rcof = (double *)calloc( 2 * n, sizeof(double) );
    if( rcof == NULL ) return( NULL );

    theta = M_PI * fcf;
    st = sin(theta);
    ct = cos(theta);

    for( k = 0; k < n; ++k )
    {
	parg = M_PI * (double)(2*k+1)/(double)(2*n);
	sparg = sin(parg);
	cparg = cos(parg);
	a = 1.0 + st*sparg;
	rcof[2*k] = -ct/a;
	rcof[2*k+1] = -st*cparg/a;
    }

    dcof = binomial_mult( n, rcof );
    free( rcof );

    dcof[1] = dcof[0];
    dcof[0] = 1.0;
    for( k = 3; k <= n; ++k )
        dcof[k] = dcof[2*k-2];
    return( dcof );
}

/**********************************************************************
  dcof_bwhp - calculates the d coefficients for a butterworth highpass 
  filter. The coefficients are returned as an array of doubles.
*/

double *dcof_bwhp( int n, double fcf )
{
    return( dcof_bwlp( n, fcf ) );
}


/**********************************************************************
  dcof_bwbp - calculates the d coefficients for a butterworth bandpass 
  filter. The coefficients are returned as an array of doubles.
*/

double *dcof_bwbp( int n, double f1f, double f2f )
{
    int k;            // loop variables
    double theta;     // M_PI * (f2f - f1f) / 2.0
    double cp;        // cosine of phi
    double st;        // sine of theta
    double ct;        // cosine of theta
    double s2t;       // sine of 2*theta
    double c2t;       // cosine 0f 2*theta
    double *rcof;     // z^-2 coefficients
    double *tcof;     // z^-1 coefficients
    double *dcof;     // dk coefficients
    double parg;      // pole angle
    double sparg;     // sine of pole angle
    double cparg;     // cosine of pole angle
    double a;         // workspace variables

    cp = cos(M_PI * (f2f + f1f) / 2.0);
    theta = M_PI * (f2f - f1f) / 2.0;
    st = sin(theta);
    ct = cos(theta);
    s2t = 2.0*st*ct;        // sine of 2*theta
    c2t = 2.0*ct*ct - 1.0;  // cosine of 2*theta

    rcof = (double *)calloc( 2 * n, sizeof(double) );
    tcof = (double *)calloc( 2 * n, sizeof(double) );

    for( k = 0; k < n; ++k )
    {
	parg = M_PI * (double)(2*k+1)/(double)(2*n);
	sparg = sin(parg);
	cparg = cos(parg);
	a = 1.0 + s2t*sparg;
	rcof[2*k] = c2t/a;
	rcof[2*k+1] = s2t*cparg/a;
	tcof[2*k] = -2.0*cp*(ct+st*sparg)/a;
	tcof[2*k+1] = -2.0*cp*st*cparg/a;
    }

    dcof = trinomial_mult( n, tcof, rcof );
    free( tcof );
    free( rcof );

    dcof[1] = dcof[0];
    dcof[0] = 1.0;
    for( k = 3; k <= 2*n; ++k )
        dcof[k] = dcof[2*k-2];
    return( dcof );
}

/**********************************************************************
  dcof_bwbs - calculates the d coefficients for a butterworth bandstop 
  filter. The coefficients are returned as an array of doubles.
*/

double *dcof_bwbs( int n, double f1f, double f2f )
{
    int k;            // loop variables
    double theta;     // M_PI * (f2f - f1f) / 2.0
    double cp;        // cosine of phi
    double st;        // sine of theta
    double ct;        // cosine of theta
    double s2t;       // sine of 2*theta
    double c2t;       // cosine 0f 2*theta
    double *rcof;     // z^-2 coefficients
    double *tcof;     // z^-1 coefficients
    double *dcof;     // dk coefficients
    double parg;      // pole angle
    double sparg;     // sine of pole angle
    double cparg;     // cosine of pole angle
    double a;         // workspace variables

    cp = cos(M_PI * (f2f + f1f) / 2.0);
    theta = M_PI * (f2f - f1f) / 2.0;
    st = sin(theta);
    ct = cos(theta);
    s2t = 2.0*st*ct;        // sine of 2*theta
    c2t = 2.0*ct*ct - 1.0;  // cosine 0f 2*theta

    rcof = (double *)calloc( 2 * n, sizeof(double) );
    tcof = (double *)calloc( 2 * n, sizeof(double) );  

    for( k = 0; k < n; ++k )
    {
	parg = M_PI * (double)(2*k+1)/(double)(2*n);
	sparg = sin(parg);
	cparg = cos(parg);
	a = 1.0 + s2t*sparg;
	rcof[2*k] = c2t/a;
	rcof[2*k+1] = -s2t*cparg/a;
	tcof[2*k] = -2.0*cp*(ct+st*sparg)/a;
	tcof[2*k+1] = 2.0*cp*st*cparg/a;
    }

    dcof = trinomial_mult( n, tcof, rcof );
    free( tcof );
    free( rcof );

    dcof[1] = dcof[0];
    dcof[0] = 1.0;
    for( k = 3; k <= 2*n; ++k )
        dcof[k] = dcof[2*k-2];
    return( dcof );
}

/**********************************************************************
  ccof_bwlp - calculates the c coefficients for a butterworth lowpass 
  filter. The coefficients are returned as an array of integers.
*/

int *ccof_bwlp( int n )
{
    int *ccof;
    int m;
    int i;

    ccof = (int *)calloc( n+1, sizeof(int) );
    if( ccof == NULL ) return( NULL );

    ccof[0] = 1;
    ccof[1] = n;
    m = n/2;
    for( i=2; i <= m; ++i)
    {
        ccof[i] = (n-i+1)*ccof[i-1]/i;
        ccof[n-i]= ccof[i];
    }
    ccof[n-1] = n;
    ccof[n] = 1;

    return( ccof );
}

/**********************************************************************
  ccof_bwhp - calculates the c coefficients for a butterworth highpass 
  filter. The coefficients are returned as an array of integers.
*/

int *ccof_bwhp( int n )
{
    int *ccof;
    int i;

    ccof = ccof_bwlp( n );
    if( ccof == NULL ) return( NULL );

    for( i = 0; i <= n; ++i)
        if( i % 2 ) ccof[i] = -ccof[i];

    return( ccof );
}

/**********************************************************************
  ccof_bwbp - calculates the c coefficients for a butterworth bandpass 
  filter. The coefficients are returned as an array of integers.
*/

int *ccof_bwbp( int n )
{
    int *tcof;
    int *ccof;
    int i;

    ccof = (int *)calloc( 2*n+1, sizeof(int) );
    if( ccof == NULL ) return( NULL );

    tcof = ccof_bwhp(n);
    if( tcof == NULL ) return( NULL );

    for( i = 0; i < n; ++i)
    {
        ccof[2*i] = tcof[i];
        ccof[2*i+1] = 0.0;
    }
    ccof[2*n] = tcof[n];

    free( tcof );
    return( ccof );
}

/**********************************************************************
  ccof_bwbs - calculates the c coefficients for a butterworth bandstop 
  filter. The coefficients are returned as an array of integers.
*/

double *ccof_bwbs( int n, double f1f, double f2f )
{
    double alpha;
    double *ccof;
    int i, j;

    alpha = -2.0 * cos(M_PI * (f2f + f1f) / 2.0) / cos(M_PI * (f2f - f1f) / 2.0);

    ccof = (double *)calloc( 2*n+1, sizeof(double) );

    ccof[0] = 1.0;

    ccof[2] = 1.0;
    ccof[1] = alpha;
  
    for( i = 1; i < n; ++i )
    {
	ccof[2*i+2] += ccof[2*i];
	for( j = 2*i; j > 1; --j )
	    ccof[j+1] += alpha * ccof[j] + ccof[j-1];

	ccof[2] += alpha * ccof[1] + 1.0;
	ccof[1] += alpha;
    }

    return( ccof );
}

/**********************************************************************
  sf_bwlp - calculates the scaling factor for a butterworth lowpass filter.
  The scaling factor is what the c coefficients must be multiplied by so
  that the filter response has a maximum value of 1.
*/

double sf_bwlp( int n, double fcf )
{
    int m, k;         // loop variables
    double omega;     // M_PI * fcf
    double fomega;    // function of omega
    double parg0;     // zeroth pole angle
    double sf;        // scaling factor

    omega = M_PI * fcf;
    fomega = sin(omega);
    parg0 = M_PI / (double)(2*n);

    m = n / 2;
    sf = 1.0;
    for( k = 0; k < n/2; ++k )
        sf *= 1.0 + fomega * sin((double)(2*k+1)*parg0);

    fomega = sin(omega / 2.0);

    if( n % 2 ) sf *= fomega + cos(omega / 2.0);
    sf = pow( fomega, n ) / sf;

    return(sf);
}

/**********************************************************************
  sf_bwhp - calculates the scaling factor for a butterworth highpass filter.
  The scaling factor is what the c coefficients must be multiplied by so
  that the filter response has a maximum value of 1.
*/

double sf_bwhp( int n, double fcf )
{
    int m, k;         // loop variables
    double omega;     // M_PI * fcf
    double fomega;    // function of omega
    double parg0;     // zeroth pole angle
    double sf;        // scaling factor

    omega = M_PI * fcf;
    fomega = sin(omega);
    parg0 = M_PI / (double)(2*n);

    m = n / 2;
    sf = 1.0;
    for( k = 0; k < n/2; ++k )
        sf *= 1.0 + fomega * sin((double)(2*k+1)*parg0);

    fomega = cos(omega / 2.0);

    if( n % 2 ) sf *= fomega + sin(omega / 2.0);
    sf = pow( fomega, n ) / sf;

    return(sf);
}

/**********************************************************************
  sf_bwbp - calculates the scaling factor for a butterworth bandpass filter.
  The scaling factor is what the c coefficients must be multiplied by so
  that the filter response has a maximum value of 1.
*/

double sf_bwbp( int n, double f1f, double f2f )
{
    int k;            // loop variables
    double ctt;       // cotangent of theta
    double sfr, sfi;  // real and imaginary parts of the scaling factor
    double parg;      // pole angle
    double sparg;     // sine of pole angle
    double cparg;     // cosine of pole angle
    double a, b, c;   // workspace variables

    ctt = 1.0 / tan(M_PI * (f2f - f1f) / 2.0);
    sfr = 1.0;
    sfi = 0.0;

    for( k = 0; k < n; ++k )
    {
	parg = M_PI * (double)(2*k+1)/(double)(2*n);
	sparg = ctt + sin(parg);
	cparg = cos(parg);
	a = (sfr + sfi)*(sparg - cparg);
	b = sfr * sparg;
	c = -sfi * cparg;
	sfr = b - c;
	sfi = a - b - c;
    }

    return( 1.0 / sfr );
}

/**********************************************************************
  sf_bwbs - calculates the scaling factor for a butterworth bandstop filter.
  The scaling factor is what the c coefficients must be multiplied by so
  that the filter response has a maximum value of 1.
*/

double sf_bwbs( int n, double f1f, double f2f )
{
    int k;            // loop variables
    double tt;        // tangent of theta
    double sfr, sfi;  // real and imaginary parts of the scaling factor
    double parg;      // pole angle
    double sparg;     // sine of pole angle
    double cparg;     // cosine of pole angle
    double a, b, c;   // workspace variables

    tt = tan(M_PI * (f2f - f1f) / 2.0);
    sfr = 1.0;
    sfi = 0.0;

    for( k = 0; k < n; ++k )
    {
	parg = M_PI * (double)(2*k+1)/(double)(2*n);
	sparg = tt + sin(parg);
	cparg = cos(parg);
	a = (sfr + sfi)*(sparg - cparg);
	b = sfr * sparg;
	c = -sfi * cparg;
	sfr = b - c;
	sfi = a - b - c;
    }

    return( 1.0 / sfr );
}