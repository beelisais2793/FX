#pragma once
#include "SoundObject.hpp"

namespace FX::Distortion::Diode
{
    ///////////////////////////////////////////////////////////////////////////
    // Diode Saturation
    ///////////////////////////////////////////////////////////////////////////
    inline DspFloatType Diode(DspFloatType x, DspFloatType Vt = 0.0253,DspFloatType eta = 1.68,DspFloatType Is = .105)
    {
        return Is * (exp(0.1*x/(eta*Vt))-1);
    }


    struct DiodeClipperNR : public FunctionProcessor
    {
        DspFloatType controlledR;
        DspFloatType Id, C, Ve, Vp, R, err;
        DspFloatType Fs, T;
        DspFloatType vNom, vDenom;
        DspFloatType vin;
        DspFloatType vout, voutTemp, voutOld;
        DspFloatType beta, betaM1;    
        int oversample;
        std::vector<DspFloatType> blockInput, blockOutput, blockOutputDownsampled;
        DspFloatType oldBlockOutput;
        //juce::dsp::ProcessorDuplicator< juce::dsp::IIR::Filter <DspFloatType>, juce::dsp::IIR::Coefficients<DspFloatType>> lowPassFilter;

        DiodeClipperNR() : FunctionProcessor()
        {
            // Use this method as the place to do any pre-playback
            // initialisation that you need..
            //oversampling->reset();
            //oversampling->initProcessing(static_cast<size_t> (samplesPerBlock));
            oversample = 16;
            // Set the constants
            Fs = sampleRate;
            T = 1 / Fs;
            C = 100e-9;
            R = 1e3;
            Vp = 0.17;
            Ve = 0.023;
            Id = 2.52e-9;
            err = 0.1e-3; // err for stopping iterations
            // set controlled values to starting values (redundant maybe delit later)
            controlledR = 1.0;
            voutOld = 0;
            beta = 0.125;
            betaM1 = 1 - beta;
        }
        DspFloatType gdExp(DspFloatType vc)
        {
            return Id * (std::exp(vc / (2.0 * Ve)) - 1.0);
        }
        DspFloatType gdExpDiff(DspFloatType vc)
        {
            return (Id * std::exp(vc / (2.0 * Ve))) / (2.0 * Ve);
        }
        DspFloatType limiter(DspFloatType val)
        {
            if (val < -1.0)
            {
                val = -1.0;
                return val;
            }
            else if (val > 1.0)
            {
                val = 1.0;
                return val;
            }
            return val;
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {
            voutTemp = 1;
            vout = 0;
            vin = controlledR * I;
            
            int itter = 0;
            while (std::abs(voutTemp - vout) > err && itter < 20) {
                voutTemp = vout;
                vNom = T * voutTemp * R * gdExpDiff(-voutTemp) + T * R * gdExp(-voutTemp) + voutOld * R * C + T * (gdExpDiff(voutTemp) * R * voutTemp - R * gdExp(voutTemp) + vin);
                vDenom = T * R * gdExpDiff(voutTemp) + T * R * gdExpDiff(-voutTemp) + R * C + T;
                vout = vNom / vDenom;
                itter++;
            }        
            voutOld = vout;
            
            return limiter(vout);
        }
    };


    struct DiodeClipperFP : public FunctionProcessor
    {
        DspFloatType controlledR;
        DspFloatType Id, C,  Ve, Vp, R, err;
        DspFloatType Fs, T;
        
        DspFloatType vin;
        DspFloatType vout, voutTemp, voutOld;

        int upsamplingScale;

        //juce::dsp::ProcessorDuplicator< juce::dsp::IIR::Filter <DspFloatType>, juce::dsp::IIR::Coefficients<DspFloatType>> lowPassFilter;
        
        DspFloatType gdExpDiff(DspFloatType vc)
        {
            return (Id * std::exp(vc / (2.0 * Ve))) / (2.0 * Ve);
        }
        DspFloatType gdExp(DspFloatType vc) 
        {
            return Id * (std::exp(vc / (2 * Ve)) - 1);
        }
        DspFloatType gdPoly(DspFloatType vc)
        {
            return Vp * vc*vc*vc*vc * Heaviside(vc);
        }
        DspFloatType Heaviside(DspFloatType vc)
        {
            if (vc >= 0)
                return 1;
            else return 0;
        }
        DspFloatType limiter(DspFloatType val)
        {
            if (val < -1.0)
            {
                val = -1.0;
                return val;
            }
            else if (val > 1.0)
            {
                val = 1.0;
                return val;
            }
            return val;
        }
        DiodeClipperFP() : FunctionProcessor()
        {
            //oversampling->reset();
            //oversampling->initProcessing(static_cast<size_t> (samplesPerBlock));

            //juce::dsp::ProcessSpec spec;
            //spec.sampleRate = sampleRate * 16;
            //spec.maximumBlockSize = samplesPerBlock * 15;
            //spec.numChannels = getTotalNumOutputChannels();

            //lowPassFilter.prepare(spec);
            //lowPassFilter.reset();
            // Set the constants
            Fs = sampleRate;
            T = 1 / Fs;
            C = 100e-9;
            R = 1e3;
            Vp = 0.17;
            Ve = 0.023;
            Id = 2.52e-9;
            err = 0.1e-3; // err for stopping iterations
            // set controlled values to starting values (redundant maybe delit later)
            controlledR = 1.0;
            voutOld = 0;
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {
            int itter=0;
            voutTemp = 1;
            vout = 0;
            vin = controlledR * I;
            while (std::abs(voutTemp - vout) > err && itter++ < 20) {
                voutTemp = vout;
                vout = R * T / (R * C + T) * (gdExp(-voutTemp) - gdExp(voutTemp)) + T / (R * C + T) * vin + R * C / (R * C + T) * voutOld;
            }
            
            voutOld = vout;
            //vout = vin;
            vout = limiter(vout);
            return vout;            
        }
    };
}

