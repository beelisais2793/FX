#pragma once

#include "Distortion/Diode.hpp"
#include "Distortion/DiodeSim.hpp"

namespace Distortion::Diode
{
    struct DiodeSaturation : public MonoFXProcessor
    {
        float pre = 0.5;
        float dist = 0.9;
        Filters::CSmoothFilter preSmooth,distSmooth;

        DiodeSaturation()
        : MonoFXProcessor(), 
        preSmooth(sampleRate,0.01),
        distSmooth(sampleRate,0.01)
        {

        }

        void setPreGain(float p)  { pre = p; }
        void setDistGain(float d) { dist = d; }

        void setPreGainDB(float p)  { pre = pow(10,p/20.0f); }
        void setDistGainDB(float d) { dist = pow(10,d/20.0f); }

        void PreProcess(size_t nSamples, float * afTempIn, float * afTempOut)
        {
            const EDiodeType aDiodes[] = {DIO_ZENER_2V4, DIO_SCHOTTKY};                
            saturationSigned(afTempIn, afTempOut, nSamples, SAT_NON_INVERTING, 2, aDiodes, POT_LINEAR, 1000.0f, 50000.0f, 100.0f, logPot(pre) );                        
        }
        void PostProcess(size_t nSamples, float * afTempIn, float * afTempOut)
        {
            const EDiodeType aDiodes[] = {DIO_ZENER_2V4, DIO_SCHOTTKY};
            saturationSigned(afTempIn, afTempOut, nSamples, SAT_INVERTING, 2, aDiodes, POT_LINEAR, 1000.0f, 100000.0f, 100.0f, logPot(dist) );        
        }
        void ProcessBlock(size_t nSamples, float * afTempIn, float * afTempOut)
        {
            const EDiodeType aDiodes[] = {DIO_ZENER_2V4, DIO_LED};
            const EDiodeType aDiodes2[] = {DIO_ZENER_2V4, DIO_LED};
            const EDiodeType aDiodes3[] = {DIO_ZENER_3V0, DIO_LED};
            const EDiodeType aDiodes4[] = {DIO_ZENER_3V9, DIO_LED};
            saturationSigned(afTempIn, afTempOut, nSamples, SAT_NON_INVERTING, 2, aDiodes, POT_LINEAR, 1000.0f, 50000.0f, 100.0f, logPot(pre) );                        
            saturationSigned(afTempOut, afTempOut, nSamples, SAT_INVERTING, 2, aDiodes2, POT_LINEAR, 1000.0f, 100000.0f, 100.0f, logPot(dist) );        
            saturationSigned(afTempOut, afTempOut, nSamples, SAT_NON_INVERTING, 2, aDiodes3, POT_LINEAR, 1000.0f, 50000.0f, 100.0f, logPot(pre) );                        
            saturationSigned(afTempOut, afTempOut, nSamples, SAT_INVERTING, 2, aDiodes4, POT_LINEAR, 1000.0f, 100000.0f, 100.0f, logPot(dist) );                
        }    
    };
}