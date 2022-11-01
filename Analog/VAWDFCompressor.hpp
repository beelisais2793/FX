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

namespace Analog::WDFCompressor
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

    //-----------------------------------------------------------------------------------
    //---------------------------- Helper Virtual Class ---------------------------------
    //-----------------------------------------------------------------------------------
    int wdfSubmoduleBase::setRootMatrData(matData* rootMats, DspFloatType* Rp)
    {
        if (rootMats->Emat.is_empty())
        {
            return -1;
        }
        if (rootMats->Emat.n_cols != 1)
        {
            return -1;
        }
        if (rootMats->Emat.n_rows != 1)
        {
            return -1;
        }
        
        rootMats->Emat.at(0,0) = 1;
        
        
        //------------------------- F matrix -------------------------
        if (rootMats->Fmat.is_empty())
        {
            return -1;
        }
        if (rootMats->Fmat.n_cols != 1)
        {
            return -1;
        }
        if (rootMats->Fmat.n_rows != 1)
        {
            return -1;
        }
        
        rootMats->Fmat.at(0,0) = -Rp[0];
        
        
        //------------------------- M matrix -------------------------
        if (rootMats->Mmat.is_empty())
        {
            return -1;
        }
        if (rootMats->Mmat.n_cols != 1)
        {
            return -1;
        }
        if (rootMats->Mmat.n_rows != 1)
        {
            return -1;
        }
        
        rootMats->Mmat.at(0,0) = 1;
        
        //------------------------- N matrix -------------------------
        if (rootMats->Nmat.is_empty())
        {
            return -1;
        }
        if (rootMats->Nmat.n_cols != 1)
        {
            return -1;
        }
        if (rootMats->Nmat.n_rows != 1)
        {
            return -1;
        }
        
        rootMats->Nmat.at(0,0) = -2*Rp[0];
        
        //nowTime = String::formatted(("%012.9f"), (float)(Time::getMillisecondCounterHiRes() - startupTime));
        //myLogger->writeToLog("\n->"+nowTime+" ms: End setRootMatrixData. Duration: "+String(nowTime)+" ms");
        
        return 0;
    }

    void wdfSubmoduleBase::setInputValue(DspFloatType signalIn) {}

    DspFloatType wdfSubmoduleBase::getOutputValue() {return 0;}

    void wdfSubmoduleBase::setParam(size_t paramID, DspFloatType paramValue) {}

    const char* wdfSubmoduleBase::getTreeIdentifier()
    {
        return m_sTreeName.c_str();
    }

    //-----------------------------------------------------------------------------------
    //------------------------- One Pole High Pass Filter -------------------------------
    //-----------------------------------------------------------------------------------
    //
    // WDF TREE:
    // [] = component
    // () = adapter
    //
    //          [ideal voltage source (input)]
    //                       |
    //                    (series)
    //                    |      |
    //             [resistor]   [cap]
    //
    wdfOnePoleHighPass::wdfOnePoleHighPass()
    {
        m_pInputSource.reset(new wdfIdealVSource(1.));
        m_pCap.reset(new wdfTerminatedCap(1.,1.));
        m_pRes.reset(new wdfTerminatedRes(2e3));
        m_pSeriesAdapter.reset(new wdfTerminatedSeries(m_pRes.get(),m_pCap.get()));
        
        m_pCap.get()->prevA = 0.0000;
        
        subtreeCount = 1;
        subtreeEntryNodes = new wdfTreeNode*[subtreeCount];
        subtreeEntryNodes[0] = m_pSeriesAdapter.get();
        
        root.reset( new wdfRootSimple(m_pInputSource.get()) );
        Rp = new DspFloatType[subtreeCount] ();
        
        m_sTreeName = "High Pass";
    }

    wdfOnePoleHighPass::~wdfOnePoleHighPass()
    {}

    void wdfOnePoleHighPass::processSample(float sampleIn, float & sampleOut)
    {
        m_pInputSource->Vs = sampleIn;
        cycleWave();
        sampleOut = (float)m_pRes->upPort->getPortCurrent();
    }

    void wdfOnePoleHighPass::reset()
    {
        m_pCap.get()->prevA = 0;
        initTree();
    }

    // Setters and Getters
    void wdfOnePoleHighPass::setResComponentVal(float resVal, bool isNormalized)
    {
        if(isNormalized)
        {
            
        }
        else
        {
            m_fResComponentVal = resVal;
            m_pRes->R = resVal;
        }
    }

    void wdfOnePoleHighPass::setCapComponentVal(float capVal, bool isNormalized)
    {
        if(isNormalized)
        {
            
        }
        else
        {
            m_fCapComponentVal = capVal;
            m_pCap->C = capVal;
        }
    }

    float wdfOnePoleHighPass::getResComponentVal(bool isNormalized)
    {
        if(isNormalized)
        {
            
        }
        else
        {
            return m_fResComponentVal;
        }
        return 0;
    }

    float wdfOnePoleHighPass::getCapComponentVal(bool isNormalized)
    {
        if(isNormalized)
        {
            
        }
        else
        {
            return m_fCapComponentVal;
        }
        return 0;
    }

    //-----------------------------------------------------------------------------------
    //------------------------------- GAIN PROCESSOR ------------------------------------
    //-----------------------------------------------------------------------------------
    //
    // WDF TREE:
    // [] = component
    // () = adapter
    //
    //                            [LDR]
    //                              |
    //                          (parallel)
    //                          |       |
    //                      (parallel) [cap]
    //                      |       |
    //[resistive source (input)]   [resistor]
    //
    wdfGainProcessor::wdfGainProcessor()
    {
        m_pInputSource.reset(new wdfTerminatedResVSource(0.,1.));
        m_pCap.reset(new wdfTerminatedCap(1.,1.));
        m_pLdr.reset(new wdfUnterminatedRes(2e3));
        
        // initialize adapter moving from bottom (widest part) of tree to top (narrowest part)
        m_pParallelAdapter.reset(new wdfTerminatedParallel(m_pInputSource.get(), m_pCap.get()));
        
        m_pCap.get()->prevA = 0.0000;
        
        subtreeCount = 1;
        subtreeEntryNodes = new wdfTreeNode*[subtreeCount];
        subtreeEntryNodes[0] = m_pParallelAdapter.get();
        
        root.reset(new wdfRootSimple(m_pLdr.get()));
        Rp = new DspFloatType[subtreeCount] ();
        
        m_sTreeName = "Gain Processor";
    }

    wdfGainProcessor::~wdfGainProcessor() {}

    void wdfGainProcessor::processSample(float sampleIn, float & sampleOut)
    {
        // expecting current input, need to apply Thevenin equivalence to convert to voltage
        m_pInputSource->Vs = sampleIn * m_fResComponentVal;
        cycleWave();
        sampleOut = (float)m_pInputSource->upPort->getPortCurrent() * 100e3; // take current through resistor and multiply by output resistor
    }

    void wdfGainProcessor::reset()
    {
        m_pCap.get()->prevA = 0;
        initTree();
    }

    // Setters and Getters
    void wdfGainProcessor::setResComponentVal(float resVal, bool isNormalized)
    {
        if(isNormalized)
        {
            
        }
        else
        {
            m_fResComponentVal = resVal;
            m_pInputSource->RSer = resVal;
        }
    }

    void wdfGainProcessor::setCapComponentVal(float capVal, bool isNormalized)
    {
        if(isNormalized)
        {
            
        }
        else
        {
            m_fCapComponentVal = capVal;
            m_pCap->C = capVal;
        }
    }

    float wdfGainProcessor::getResComponentVal(bool isNormalized)
    {
        if(isNormalized)
        {
            
        }
        else
        {
            return m_fResComponentVal;
        }
        return 0;
    }

    float wdfGainProcessor::getCapComponentVal(bool isNormalized)
    {
        if(isNormalized)
        {
            
        }
        else
        {
            return m_fCapComponentVal;
        }
        return 0;
    }

    void wdfGainProcessor::setLdrComponentVal(float ldrVal)
    {
        // input is current off of diode from envelope follower, LDR curve is roughly logarithmic, so:
        // normalize --> take log --> multiply against resistor range
        
        // resistance values come from datasheet and Eichas & Zölzer paper
        const float maxResistance = 1e6;
        const float minResistance = 100;
        
        if(ldrVal > 1.f) ldrVal = 1.f;
        else if (ldrVal < 0.f) ldrVal = ldrVal * -1;
        
        // LDR curve looks roughly logarithmic. For easy of use, log function shifted to intercept (0,1) and (1,0)
        float logVal = -log10(ldrVal * 1e3  + 0.1);
        
        // Clip
        if(logVal > 1.f) logVal = 1.f;
        else if (logVal < 0.f) logVal = 0.f;
        
        m_fLdrComponentVal = logVal * (maxResistance - minResistance) + minResistance;
    }

    float wdfGainProcessor::getLdrComponentVal()
    {
        return m_fLdrComponentVal;
    }
    //-----------------------------------------------------------------------------------
    //----------------------------- ENVELOPE FOLLOWER -----------------------------------
    //-----------------------------------------------------------------------------------
    //
    // WDF TREE:
    // [] = component
    // () = adapter
    //
    //                                               [LED]
    //                                                 |
    //                                             (series)
    //                                             |      |
    //                                      (series)     [resistor 2]
    //                                      |      |
    //                              (parallel)    [cap]
    //                              |        |
    // [Resistive voltage source (input)]   [resistor 1]
    //
    wdfEnvelopeFollower::wdfEnvelopeFollower()
    {
        // input hpf
        m_pHpf = new wdfOnePoleHighPass();
        m_pHpf->setResComponentVal(m_fHpfResVal);
        m_pHpf->setCapComponentVal(m_fHpfCapval);
        
        m_pInputSource.reset(new wdfTerminatedResVSource(0.,1.));
        m_pCap.reset(new wdfTerminatedCap(1.,1.));
        m_pR1.reset(new wdfTerminatedRes(2e3));
        m_pR2.reset(new wdfTerminatedRes(2e3));
        
        // initialize adapter moving from bottom (widest part) of tree to top (narrowest part)
        m_pParallelAdapter.reset(new wdfTerminatedParallel(m_pInputSource.get(), m_pR1.get()));
        m_pSeriesAdapters[kSeriesAdapterTreeLevel2].reset(new wdfTerminatedSeries(m_pParallelAdapter.get(), m_pCap.get()));
        m_pSeriesAdapters[kSeriesAdapterTreeLevel1].reset(new wdfTerminatedSeries(m_pSeriesAdapters[kSeriesAdapterTreeLevel2].get(),
                                                                                m_pR2.get()));
        
        m_pCap.get()->prevA = 0.0000;
        
        subtreeCount = 3;
        subtreeEntryNodes = new wdfTreeNode*[subtreeCount];
        subtreeEntryNodes[0] = m_pSeriesAdapters[kSeriesAdapterTreeLevel1].get();
        subtreeEntryNodes[1] = m_pSeriesAdapters[kSeriesAdapterTreeLevel2].get();
        subtreeEntryNodes[2] = m_pParallelAdapter.get();
        
        root.reset(new wdfRootNL(subtreeCount, {DIODE}, 1));
        Rp = new DspFloatType[subtreeCount] ();
        m_sTreeName = "Envelope Follower";
    }

    wdfEnvelopeFollower::~wdfEnvelopeFollower(){}

    // RETURNS A CONTROL VOLTAGE, NOT AUDIO 
    void wdfEnvelopeFollower::processSample(float sampleIn, float &sampleOut)
    {
        m_pHpf->processSample(sampleIn, sampleIn);
        // need to get Thevenin equivalent
        m_pInputSource->Vs = sampleIn * m_fR1ComponentVal;
        cycleWave();
        sampleOut = (float)m_pSeriesAdapters[kSeriesAdapterTreeLevel1]->upPort->getPortVoltage();
    }

    void wdfEnvelopeFollower::reset()
    {
        m_pCap.get()->prevA = 0;
        m_pHpf->reset();
        m_pHpf->adaptTree();
        initTree();
    }

    void wdfEnvelopeFollower::setSampleRate(float sampleRate)
    {
        setSampleRate(sampleRate);
        m_pHpf->setSamplerate(sampleRate);
    }

    // Setters and Getters
    void wdfEnvelopeFollower::setR1ComponentVal(float r1Val, bool isNormalized)
    {
        if(isNormalized)
        {
            
        }
        else
        {
            m_fR1ComponentVal = r1Val;
            m_pR1->R = r1Val;
        }
    }

    void wdfEnvelopeFollower::setR2ComponentVal(float r2Val, bool isNormalized)
    {
        if(isNormalized)
        {
            
        }
        else
        {
            m_fR2ComponentVal = r2Val;
            m_pR2->R = r2Val;
        }
    }

    void wdfEnvelopeFollower::setCapComponentVal(float capVal, bool isNormalized)
    {
        if(isNormalized)
        {
            
        }
        else
        {
            m_fCapComponentVal = capVal;
            m_pCap->C = capVal;
        }
    }

    float wdfEnvelopeFollower::getR1ComponentVal(bool isNormalized)
    {
        if(isNormalized)
        {
            
        }
        else
        {
            return m_fR1ComponentVal;
        }
        return 0;
    }

    float wdfEnvelopeFollower::getR2ComponentVal(bool isNormalized)
    {
        if(isNormalized)
        {
            
        }
        else
        {
            return m_fR2ComponentVal;
        }
        return 0;
    }

    float wdfEnvelopeFollower::getCapComponentVal(bool isNormalized)
    {
        if(isNormalized)
        {
            
        }
        else
        {
            return m_fCapComponentVal;
        }
        return 0;
    }


}