/*
  ==============================================================================
    CompressorSubmodules.h
    Created: 21 Jun 2017 4:22:41pm
    Author:  Davis
  ==============================================================================
*/

#pragma once

#include "rt-wdf.h"

using namespace arma;

namespace wdfSubmodule
{
    class wdfSubmoduleBase : public wdfTree
    {
    public:
        virtual int         setRootMatrData(matData* rootMats, double* Rp);
        virtual void        setInputValue(double signalIn);
        virtual double      getOutputValue();
        virtual const char* getTreeIdentifier();
        virtual void        setParam(size_t paramID, double paramValue);
        
    protected:
        std::string m_sTreeName = "";
    };
    
    ///////////////////////////////////////////////////
    //                 HIGH PASS                     //
    ///////////////////////////////////////////////////
    class wdfOnePoleHighPass : public wdfSubmoduleBase
    {
    public:
        wdfOnePoleHighPass();
        ~wdfOnePoleHighPass();
        
        void        processSample(float sampleIn, float & sampleOut);
        void        reset();
        
        void        setResComponentVal(float resVal, bool isNormalized = 0);
        void        setCapComponentVal(float capVal, bool isNormalized = 0);
        float       getResComponentVal(bool isNormalized = 0);
        float       getCapComponentVal(bool isNormalized = 0);


        enum
        {
            kIndexResistor,
            kIndexCapacitor,
            kIndexNumParams
        };
        
    private:
        //-----------------------------------------------------------------------------------
        //------------------------------------ POD ------------------------------------------
        //-----------------------------------------------------------------------------------
        
        // Parameter values for resistor and capacitor. These are intended as actual component values,
        // not values normalized to 0 - 1.
        float m_fResComponentVal;
        float m_fCapComponentVal;
        
        //-----------------------------------------------------------------------------------
        //---------------------------------- CLASSES ----------------------------------------
        //-----------------------------------------------------------------------------------
        
        // Model filter input as an ideal voltage source (as such, it must be the root of the tree)
        std::unique_ptr<wdfIdealVSource> m_pInputSource;
        
        // WDF model of a capacitor
        std::unique_ptr<wdfTerminatedCap> m_pCap;
        
        // WDF model of a resistor
        std::unique_ptr<wdfTerminatedRes> m_pRes;
        
        // WDF series adapter. Necessary for connecting the resistive source to the capacitor
        std::unique_ptr<wdfTerminatedSeries> m_pSeriesAdapter;
    };
    
    ///////////////////////////////////////////////////
    //                GAIN PROCESSOR                 //
    ///////////////////////////////////////////////////
    class wdfGainProcessor : public wdfSubmoduleBase
    {
    public:
        wdfGainProcessor();
        ~wdfGainProcessor();
        
        void        processSample(float sampleIn, float & sampleOut);
        void        reset();
        
        void        setResComponentVal(float resVal, bool isNormalized = 0);
        void        setLdrComponentVal(float ldrVal);
        void        setCapComponentVal(float capVal, bool isNormalized = 0);
        float       getResComponentVal(bool isNormalized = 0);
        float       getLdrComponentVal();
        float       getCapComponentVal(bool isNormalized = 0);
    private:
        //-----------------------------------------------------------------------------------
        //------------------------------------ POD ------------------------------------------
        //-----------------------------------------------------------------------------------
        
        // Parameter values for resistor and capacitor. These are intended as actual component values,
        // not values normalized to 0 - 1.
        float m_fResComponentVal;
        float m_fCapComponentVal;
        float m_fLdrComponentVal;
        
        //-----------------------------------------------------------------------------------
        //---------------------------------- CLASSES ----------------------------------------
        //-----------------------------------------------------------------------------------
        
        // Model filter input as a resistive voltage source
        std::unique_ptr<wdfTerminatedResVSource> m_pInputSource;
        
        // WDF model of a capacitor
        std::unique_ptr<wdfTerminatedCap> m_pCap;
        
        // Light-sensitive resistor
        std::unique_ptr<wdfUnterminatedRes> m_pLdr;
        
        // WDF series adapter. Necessary for connecting components
        std::unique_ptr<wdfTerminatedParallel> m_pParallelAdapter;
    };
    
    ///////////////////////////////////////////////////
    //               ENVELOPE FOLLOWER               //
    ///////////////////////////////////////////////////
    class wdfEnvelopeFollower : public wdfSubmoduleBase
    {
    public:
        wdfEnvelopeFollower();
        ~wdfEnvelopeFollower();
        
        void        processSample(float sampleIn, float & sampleOut);
        void        reset();
        void        setSampleRate(float sampleRate);

        void        setR1ComponentVal(float r1Val, bool isNormalized = 0);
        void        setR2ComponentVal(float r2Val, bool isNormalized = 0);
        void        setCapComponentVal(float capVal, bool isNormalized = 0);
        float       getR1ComponentVal(bool isNormalized = 0);
        float       getR2ComponentVal(bool isNormalized = 0);
        float       getCapComponentVal(bool isNormalized = 0);
    private:
        //-----------------------------------------------------------------------------------
        //------------------------------------ POD ------------------------------------------
        //-----------------------------------------------------------------------------------
        
        float m_fCapComponentVal;
        float m_fR1ComponentVal;
        float m_fR2ComponentVal;
        
        //-----------------------------------------------------------------------------------
        //--------------------------------- CONSTANTS ---------------------------------------
        //-----------------------------------------------------------------------------------
        
        enum
        {
            kSeriesAdapterTreeLevel1,
            kSeriesAdapterTreeLevel2,
            kSeriesAdapterNumAdapters
        };
        const float m_fHpfCapval = 100e-9;
        const float m_fHpfResVal = 10e3;
        
        //-----------------------------------------------------------------------------------
        //---------------------------------- CLASSES ----------------------------------------
        //-----------------------------------------------------------------------------------
        wdfOnePoleHighPass * m_pHpf;
        
        // Model filter input as a resistive voltage source
        std::unique_ptr<wdfTerminatedResVSource> m_pInputSource;
        
        // WDF model of a capacitor
        std::unique_ptr<wdfTerminatedCap> m_pCap;
        
        // WDF model of a resistor
        std::unique_ptr<wdfTerminatedRes> m_pR1;
        std::unique_ptr<wdfTerminatedRes> m_pR2;
        
        // WDF adapters. Necessary for connecting components
        std::unique_ptr<wdfTerminatedSeries> m_pSeriesAdapters[kSeriesAdapterNumAdapters];
        std::unique_ptr<wdfTerminatedParallel> m_pParallelAdapter;
    };
}