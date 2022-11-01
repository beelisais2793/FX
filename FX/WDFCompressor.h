/*
  ==============================================================================
    CompressorSubmodules.h
    Created: 21 Jun 2017 4:22:41pm
    Author:  Davis
  ==============================================================================
*/

#pragma once

#include "rt-wdf.hpp"

using namespace arma;

namespace wdfSubmodule
{
    class wdfSubmoduleBase : public wdfTree
    {
    public:
        virtual int         setRootMatrData(matData* rootMats, DspFloatType* Rp);
        virtual void        setInputValue(DspFloatType signalIn);
        virtual DspFloatType      getOutputValue();
        virtual const char* getTreeIdentifier();
        virtual void        setParam(size_t paramID, DspFloatType paramValue);
        
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
        
        void        processSample(DspFloatType sampleIn, DspFloatType & sampleOut);
        void        reset();
        
        void        setResComponentVal(DspFloatType resVal, bool isNormalized = 0);
        void        setCapComponentVal(DspFloatType capVal, bool isNormalized = 0);
        DspFloatType       getResComponentVal(bool isNormalized = 0);
        DspFloatType       getCapComponentVal(bool isNormalized = 0);


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
        DspFloatType m_fResComponentVal;
        DspFloatType m_fCapComponentVal;
        
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
        
        void        processSample(DspFloatType sampleIn, DspFloatType & sampleOut);
        void        reset();
        
        void        setResComponentVal(DspFloatType resVal, bool isNormalized = 0);
        void        setLdrComponentVal(DspFloatType ldrVal);
        void        setCapComponentVal(DspFloatType capVal, bool isNormalized = 0);
        DspFloatType       getResComponentVal(bool isNormalized = 0);
        DspFloatType       getLdrComponentVal();
        DspFloatType       getCapComponentVal(bool isNormalized = 0);
    private:
        //-----------------------------------------------------------------------------------
        //------------------------------------ POD ------------------------------------------
        //-----------------------------------------------------------------------------------
        
        // Parameter values for resistor and capacitor. These are intended as actual component values,
        // not values normalized to 0 - 1.
        DspFloatType m_fResComponentVal;
        DspFloatType m_fCapComponentVal;
        DspFloatType m_fLdrComponentVal;
        
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
        
        void        processSample(DspFloatType sampleIn, DspFloatType & sampleOut);
        void        reset();
        void        setSampleRate(DspFloatType sampleRate);

        void        setR1ComponentVal(DspFloatType r1Val, bool isNormalized = 0);
        void        setR2ComponentVal(DspFloatType r2Val, bool isNormalized = 0);
        void        setCapComponentVal(DspFloatType capVal, bool isNormalized = 0);
        DspFloatType       getR1ComponentVal(bool isNormalized = 0);
        DspFloatType       getR2ComponentVal(bool isNormalized = 0);
        DspFloatType       getCapComponentVal(bool isNormalized = 0);
    private:
        //-----------------------------------------------------------------------------------
        //------------------------------------ POD ------------------------------------------
        //-----------------------------------------------------------------------------------
        
        DspFloatType m_fCapComponentVal;
        DspFloatType m_fR1ComponentVal;
        DspFloatType m_fR2ComponentVal;
        
        //-----------------------------------------------------------------------------------
        //--------------------------------- CONSTANTS ---------------------------------------
        //-----------------------------------------------------------------------------------
        
        enum
        {
            kSeriesAdapterTreeLevel1,
            kSeriesAdapterTreeLevel2,
            kSeriesAdapterNumAdapters
        };
        const DspFloatType m_fHpfCapval = 100e-9;
        const DspFloatType m_fHpfResVal = 10e3;
        
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