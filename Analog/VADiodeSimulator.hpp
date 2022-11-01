#pragma once

#include "FX/Diode.hpp"
#include "FX/DiodeSim.hpp"
#include "FX/CSmoothFilters.hpp"

namespace Analog::Distortion::Diode
{
    struct DiodeSaturation : public MonoFXProcessor
    {
        DspFloatType pre = 0.5;
        DspFloatType dist = 0.9;
        FX::Filters::Smoothers::CSmoothFilter preSmooth,distSmooth;

        DiodeSaturation()
        : MonoFXProcessor(), 
        preSmooth(sampleRate,0.01),
        distSmooth(sampleRate,0.01)
        {

        }

        void setPreGain(DspFloatType p)  { pre = p; }
        void setDistGain(DspFloatType d) { dist = d; }

        void setPreGainDB(DspFloatType p)  { pre = std::pow(10,p/20.0f); }
        void setDistGainDB(DspFloatType d) { dist = std::pow(10,d/20.0f); }

        void PreProcess(size_t nSamples, DspFloatType * afTempIn, DspFloatType * afTempOut)
        {
            const ::Distortion::Diode::EDiodeType aDiodes[] = {::Distortion::Diode::DIO_ZENER_2V4, ::Distortion::Diode::DIO_SCHOTTKY};                
            saturationSigned(afTempIn, afTempOut, nSamples, SAT_NON_INVERTING, 2, aDiodes, POT_LINEAR, 1000.0f, 50000.0f, 100.0f, logPot(pre) );                        
        }
        void PostProcess(size_t nSamples, DspFloatType * afTempIn, DspFloatType * afTempOut)
        {
            const ::Distortion::Diode::EDiodeType aDiodes[] = {DIO_ZENER_2V4, DIO_SCHOTTKY};
            saturationSigned(afTempIn, afTempOut, nSamples, SAT_INVERTING, 2, aDiodes, POT_LINEAR, 1000.0f, 100000.0f, 100.0f, logPot(dist) );        
        }
        void ProcessBlock(size_t nSamples, DspFloatType * afTempIn, DspFloatType * afTempOut)
        {
            const ::Distortion::Diode::EDiodeType aDiodes[]  = {DIO_ZENER_2V4, DIO_LED};
            const ::Distortion::Diode::EDiodeType aDiodes2[] = {DIO_ZENER_2V4, DIO_LED};
            const ::Distortion::Diode::EDiodeType aDiodes3[] = {DIO_ZENER_3V0, DIO_LED};
            const ::Distortion::Diode::EDiodeType aDiodes4[] = {DIO_ZENER_3V9, DIO_LED};
            saturationSigned(afTempIn, afTempOut, nSamples, SAT_NON_INVERTING, 2, aDiodes, POT_LINEAR, 1000.0f, 50000.0f, 100.0f, logPot(pre) );                        
            saturationSigned(afTempOut, afTempOut, nSamples, SAT_INVERTING, 2, aDiodes2, POT_LINEAR, 1000.0f, 100000.0f, 100.0f, logPot(dist) );        
            saturationSigned(afTempOut, afTempOut, nSamples, SAT_NON_INVERTING, 2, aDiodes3, POT_LINEAR, 1000.0f, 50000.0f, 100.0f, logPot(pre) );                        
            saturationSigned(afTempOut, afTempOut, nSamples, SAT_INVERTING, 2, aDiodes4, POT_LINEAR, 1000.0f, 100000.0f, 100.0f, logPot(dist) );                
        }    
    };
}