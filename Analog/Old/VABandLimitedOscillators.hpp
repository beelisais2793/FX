#pragma once


extern double sampleRate;
extern double invSampleRate;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// B-A-N-D-L-I-M-I-T-E-D O-S-C-I-L-L-A-T-O-R-S
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "SoundObject.hpp"
#include "FX/OnePole.hpp"
#include "FX/Filters.h"
#include "FX/PolyBLEP.hpp"
#include "FX/minBLEP.hpp"

namespace Analog::Oscillators
{
    //////////////////////////////////////////////
    // Old Blit it works
    //////////////////////////////////////////////
    struct BlitSaw : public OscillatorProcessor
    {
        //! Class constructor.
        BlitSaw( double frequency = 220.0 ) : OscillatorProcessor()
        {
            nHarmonics_ = 0;
            offset = 0;
            reset();
            setFrequency( frequency );
            block.setFc(10.0f/sampleRate);
            gain = 1;
        }

        //! Class destructor.
        ~BlitSaw() = default;

        //! Resets the oscillator state and phase to 0.
        void reset()
        {
            phase_ = 0.0f;
            state_ = 0.0;
            y = 0.0;
        }

        //! Set the sawtooth oscillator rate in terms of a frequency in Hz.
        void setFrequency( double frequency )
        {
            p_ = sampleRate / frequency;
            C2_ = 1 / p_;
            rate_ = M_PI * C2_;
            updateHarmonics();
        }

        void setHarmonics( unsigned int nHarmonics = 0 )
        {
            nHarmonics_ = nHarmonics;
            this->updateHarmonics();        
            state_ = -0.5 * a_;
        }

        
        //! Return the last computed output value.
        double lastOut( void ) const { return y; };

        void setGain(double g) {
            gain = g;
        }
        
        //! Compute and return one output sample.
        
        // blit = sin(m * phase) / (p * sin(phase));

        double Tick( double I=1, double A=1, double X=0, double Y=0 )
        {     
            // I = index
            // X = FM
            // Y = PM
            double tmp, denominator = sin( phase_ );
            if ( fabs(denominator) <= std::numeric_limits<double>::epsilon() )
                tmp = a_;
            else {
                tmp =  sin( m_ * phase_ );
                tmp /= p_ * denominator;
            }

            tmp += state_ - C2_;
            state_ = tmp * 0.995;
            //phase_   = x;
            phase_ += rate_;
            if ( phase_ >= M_PI ) phase_ -= M_PI;

            double out = tmp;
            y -= block.process(y);
            return y;
        }

        double getPhase() { return phase_; }

        void setPhaseOffset(double o) {
            phase_ = o;    
        }

        void updateHarmonics( void )
        {
            if ( nHarmonics_ <= 0 ) {
                unsigned int maxHarmonics = (unsigned int) floor( 0.5 * p_ );
                m_ = 2 * maxHarmonics + 1;
            }
            else
                m_ = 2 * nHarmonics_ + 1;

            a_ = m_ / p_;
        }

        OnePole     block;    
        unsigned int nHarmonics_;
        unsigned int m_;
        double rate_;
        double phase_;
        double offset;
        double p_;
        double C2_;
        double a_;
        double state_;
        double y;
        double gain;
    };


    ///////////////////////////////////////////////////////////////////////////////////////
    // square is made from subtracting out of phase sawtooth waves
    ///////////////////////////////////////////////////////////////////////////////////////
    struct BlitSquare : public OscillatorProcessor
    {
        OnePole block;
        BlitSaw s1,s2;
        double _out = 0;
        double _duty = 0.5;

        BlitSquare() : OscillatorProcessor()
        {
            block.setFc(10.0f/sampleRate);
            _out = 0;
            _duty = 0.5;
            setFrequency(440.0f);
            s1.setGain(1);
            s2.setGain(1);
        }
        void setFrequency(double f)
        {
            s1.setFrequency(f);
            s2.setFrequency(f);        
        }
        void setDuty(double d)
        {
            _duty = d;
        }
        void reset() {
            s1.reset();
            s2.reset();
        }
        double Tick(double I=1, double A=1, double X=0, double Y=0) {
            double r1 = s1.Tick();        
            s2.setPhaseOffset(s1.getPhase() + _duty*M_PI);
            double r2 = s2.Tick();                
            _out = r2-r1;
            double x = _out;
            x -= block.process(x);
            return 4*x;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////////
    // triangle integrates the square
    ///////////////////////////////////////////////////////////////////////////////////////
    struct BlitTriangle : public OscillatorProcessor
    {
        OnePole b1,b2;    
        BlitSquare s1;
        double _out = 0;
        
        BlitTriangle() : OscillatorProcessor()
        {
            b1.setFc(10.0f/sampleRate);
            b2.setFc(10.0f/sampleRate);
            setFrequency(440);
        }
        void setFrequency(double f)
        {        
            s1.setFrequency(f);                
        }
        void setDuty(double d)
        {    
            s1.setDuty(d);
        }
        void reset() {
            s1.reset();
            _out = 0;
        }
        double Tick(double I=1, double A=1, double X=0, double Y=0) {
            double r1 = s1.Tick();                        
            // there's a tremendous amount of weird dc noise in this thing
            r1   -= b1.process(r1);
            // not really sure why it works but it does I think m_ = harmonic * harmonic like the fourier expansion
            _out += (r1/s1.s1.m_);                
            double x = _out;        
            return 2*(x-b2.process(x));
        }
    };


    ///////////////////////////////////////////////////////////////////////////////////////
    // The new blit
    ///////////////////////////////////////////////////////////////////////////////////////
    double BlitDSF(double phase,double m,double p, double a) 
    {
        double tmp, denominator = sin( phase );
        if ( fabs(denominator) <= std::numeric_limits<double>::epsilon() )
            tmp = a;
        else {
            tmp =  sin( m * phase );
            tmp /= p * denominator;
        }
        return tmp;
    }

    struct blitSaw : public OscillatorProcessor
    {
        OnePole block;
        unsigned int nHarmonics_;
        unsigned int m_;
        double rate_;
        double phase_;
        double offset;
        double p_;
        double C2_;
        double a_;
        double state_;
        double y;
        
        blitSaw(double sampleRate=44100.0f, double frequency=440.0f)
        : OscillatorProcessor()
        {
            nHarmonics_ = 0;
            offset = 0;
            reset();
            setFrequency( frequency );
            block.setFc(10.0f/sampleRate);        
        }

        //! Resets the oscillator state and phase to 0.
        void reset()
        {
            phase_ = 0.0f;
            state_ = 0.0;
            y = 0.0;
        }

        //! Set the sawtooth oscillator rate in terms of a frequency in Hz.
        void setFrequency( double frequency )
        {
            p_      = (sampleRate) / frequency;
            C2_     = 1 / p_;
            rate_   = M_PI * C2_;
            updateHarmonics();
        }

        void setHarmonics( unsigned int nHarmonics = 0 )
        {
            nHarmonics_ = nHarmonics;
            this->updateHarmonics();        
            state_ = -0.5 * a_;
        }

        
        //! Return the last computed output value.
        double lastOut( void ) const  { 
            return y; 
        };
        
        
        // blit = sin(m * phase) / (p * sin(phase));
        double Tick( double I=1, double A=1, double X=0, double Y=0 )
        {    
            double tmp = BlitDSF(phase_,m_,p_,a_);        
            tmp += state_ - C2_;
            state_ = tmp * 0.995;        
            phase_ += rate_;
            if ( phase_ >= M_PI ) phase_ -= M_PI;
            y = clamp(tmp,-1,1);        
            y -= block.process(y);
            return 2*(0.8*y+0.47)-1;
        }

        double getPhase() { 
            return phase_; 
        }

        void setPhaseOffset(double o) {
            phase_ = o;    
        }

        void updateHarmonics( void )
        {
            if ( nHarmonics_ <= 0 ) {
                unsigned int maxHarmonics = (unsigned int) floor( 0.5 * p_ );
                m_ = 2 * maxHarmonics + 1;
            }
            else
                m_ = 2 * nHarmonics_ + 1;

            a_ = m_ / p_;
        }
    };

    struct blitSquare : public OscillatorProcessor
    {
        OnePole block;
        unsigned int nHarmonics_;
        unsigned int m_;
        double f;
        double rate_;
        double phase_;
        double offset;
        double p_;
        double C2_;
        double a_;
        double state_;
        double y;
        double D;
        
        blitSquare(double sampleRate=44100.0f, double frequency=440.0f)
        : OscillatorProcessor()
        {
            nHarmonics_ = 0;
            offset = 0;
            reset();
            setFrequency( frequency );
            block.setFc(10.0f/sampleRate);        
            D = 0.5;
        }

        //! Resets the oscillator state and phase to 0.
        void reset()
        {
            phase_ = 0.0f;
            state_ = 0.0;
            y = 0.0;
        }

        //! Set the sawtooth oscillator rate in terms of a frequency in Hz.
        void setFrequency( double frequency )
        {
            f       = frequency;
            p_      = (sampleRate) / frequency;
            C2_     = 1 / p_;
            rate_   = M_PI * C2_;
            updateHarmonics();
        }

        void setHarmonics( unsigned int nHarmonics = 0 )
        {
            nHarmonics_ = nHarmonics;
            this->updateHarmonics();        
            state_ = -0.5 * a_;
        }

        
        //! Return the last computed output value.
        double lastOut( void ) const  { 
            return y; 
        };
        
        
        // blit = sin(m * phase) / (p * sin(phase));
        double Tick( double I=1, double A=1, double X=0, double Y=0 )
        {    
            double tmp = BlitDSF(phase_,m_,p_,a_);        
            double tmp2= BlitDSF(phase_+D*M_PI,m_,p_,a_);
            tmp      = tmp - tmp2;
            //tmp     += state_ - C2_;        
            state_ += tmp * 0.995;
            phase_ += rate_;
            if ( phase_ >= 2*M_PI ) phase_ -= 2*M_PI;
            y = state_;
            y -= block.process(y);
            return 2*((y+D)*0.7+0.15)-1;
        }

        double getPhase() { 
            return phase_; 
        }

        void setPhaseOffset(double o) {
            phase_ = o;    
        }

        void updateHarmonics( void )
        {
            if ( nHarmonics_ <= 0 ) {
                unsigned int maxHarmonics = (unsigned int) floor( 0.5 * p_ );
                m_ = 2 * maxHarmonics + 1;
            }
            else
                m_ = 2 * nHarmonics_ + 1;

            a_ = m_ / p_;
        }
    };

    struct blitTriangle : public OscillatorProcessor
    {
        blitSquare sqr;
        OnePole b1;

        double triangle;
        blitTriangle(double sampleRate=44100.0f, double frequency=440.0f) : 
        OscillatorProcessor(),
        sqr(sampleRate,frequency)
        {
            b1.setFc(10.0f/sampleRate);
            triangle = 0;
        }
        void reset() {
            triangle = 0;
        }
        void setFrequency(double f) {
            sqr.setFrequency(f);
        }
        double Tick()
        {
            double x = sqr.Tick();
            double a = 1.0 - 0.1*std::fmin(1,sqr.f/1000.0);
            triangle = a*triangle + x/sqr.p_;
            double kaka = b1.process(triangle);
            triangle -= kaka;
            return 4*triangle;
        }
    };


    /////////////////////////////////////////////////////////////////////
    // Differential Parablic Wave
    /////////////////////////////////////////////////////////////////////
    struct DPWSaw : public OscillatorProcessor
    {
        double freq,fs,inc;
        double phase,lastPhase;
        double lastValue,position;
        double scaleFactor;

        DPWSaw() : OscillatorProcessor()
        {
            freq = 440.0f;
            fs   = sampleRate;
            inc  = freq/fs;
            lastValue = phase = lastPhase = position = 0.0f;
            scaleFactor = sampleRate / (4.0f * freq);
        }
        void setFrequency(double f) {
            freq = f;
            inc  = f/fs;
            scaleFactor = sampleRate / (4.0f * freq);
        }
        double Tick()
        {                                    
            position += phase - lastPhase;
            lastPhase = phase;

            position = fmod(position, 1.0f);

            double value = position * 2 - 1;
            value = value * value;
            
            double out = scaleFactor * (value - lastValue);
            lastValue = value;

            phase = fmod(phase + inc,1.0f);
            return out;
        }   
    };

    struct DPWPulse
    {
        double freq,fs,inc;
        double phase,lastPhase;
        double lastValueA,lastValueB,position;
        double positionA,positionB;
        double scaleFactor;

        DPWPulse()
        {
            freq = 440.0f;
            fs   = sampleRate;
            inc  = freq/fs;
            lastValueA = lastValueB = phase = lastPhase = position = 0.0f;
            positionA = 0.5f;
            positionB = 0.5f;
            scaleFactor = 0.5f * sampleRate /(4.0f * freq);    
            phase = 0.5;
        }
        void setFrequency(double f) {
            freq = f;
            inc  = f/fs;
            scaleFactor = 0.5f * sampleRate /(4.0f * freq);    
        }
        void setDuty(double d) {
            phase = clamp(d,0.01f,0.99f);
        }
        double Tick(double I=1, double A=1, double X=0, double Y=0) {
                            
            positionB += phase - lastPhase;
            lastPhase = phase;

            positionA = fmod(positionA, 1.0f);
            positionB = fmod(positionB, 1.0f);

            double valueA = positionA * 2.0f - 1.0f;
            double valueB = positionB * 2.0f - 1.0f;
            valueA = valueA * valueA;
            valueB = valueB * valueB;
            double out = ((valueA - lastValueA) -(valueB - lastValueB)) * scaleFactor;
            lastValueA = valueA;
            lastValueB = valueB;

            positionA += freq * invSampleRate;
            positionB += freq * invSampleRate;

            return out;        
        }
    };

    struct DPWTriangle
    {
        double freq,fs,inc;
        double phase,lastPhase;
        double lastValue,position;    
        double scaleFactor;

        DPWTriangle()
        {
            freq = 440.0f;
            fs   = sampleRate;
            inc  = freq/fs;
            lastValue = phase = lastPhase = position = 0.0f;
            position = 0.0f;        
            scaleFactor =  sampleRate / (2.0f * freq);
            phase = 0.5;
        }
        void setFrequency(double f) {
            freq = f;
            inc  = f/fs;
            scaleFactor =  sampleRate / (2.0f * freq);
        }
        void setDuty(double d) {
            phase = clamp(d,0.01f,0.99f);
        }
        double Tick()
        {        
            position += phase - lastPhase;
            lastPhase = phase;
            position = fmod(position, 1.0f);                
            double out = std::abs(position - 0.5) * 4 - 1;                
            position += freq * invSampleRate;        
            return out;
        }
    };
}