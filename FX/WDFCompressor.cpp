/*
  ==============================================================================
    CompressorSubmodules.cpp
    Created: 21 Jun 2017 4:22:41pm
    Author:  Davis
    Eichas & Zölzer paper used for LDR reference:
    Modeling of an Optocoupler-Based Audio Dynamic Range Control Circuit
    http://proceedings.spiedigitallibrary.org/proceeding.aspx?articleid=2565611
  ==============================================================================
*/

#include "CompressorSubmodules.h"

using namespace wdfSubmodule;

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
    
    //nowTime = String::formatted(("%012.9f"), (DspFloatType)(Time::getMillisecondCounterHiRes() - startupTime));
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

void wdfOnePoleHighPass::processSample(DspFloatType sampleIn, DspFloatType & sampleOut)
{
    m_pInputSource->Vs = sampleIn;
    cycleWave();
    sampleOut = (DspFloatType)m_pRes->upPort->getPortCurrent();
}

void wdfOnePoleHighPass::reset()
{
    m_pCap.get()->prevA = 0;
    initTree();
}

// Setters and Getters
void wdfOnePoleHighPass::setResComponentVal(DspFloatType resVal, bool isNormalized)
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

void wdfOnePoleHighPass::setCapComponentVal(DspFloatType capVal, bool isNormalized)
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

DspFloatType wdfOnePoleHighPass::getResComponentVal(bool isNormalized)
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

DspFloatType wdfOnePoleHighPass::getCapComponentVal(bool isNormalized)
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

void wdfGainProcessor::processSample(DspFloatType sampleIn, DspFloatType & sampleOut)
{
    // expecting current input, need to apply Thevenin equivalence to convert to voltage
    m_pInputSource->Vs = sampleIn * m_fResComponentVal;
    cycleWave();
    sampleOut = (DspFloatType)m_pInputSource->upPort->getPortCurrent() * 100e3; // take current through resistor and multiply by output resistor
}

void wdfGainProcessor::reset()
{
    m_pCap.get()->prevA = 0;
    initTree();
}

// Setters and Getters
void wdfGainProcessor::setResComponentVal(DspFloatType resVal, bool isNormalized)
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

void wdfGainProcessor::setCapComponentVal(DspFloatType capVal, bool isNormalized)
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

DspFloatType wdfGainProcessor::getResComponentVal(bool isNormalized)
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

DspFloatType wdfGainProcessor::getCapComponentVal(bool isNormalized)
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

void wdfGainProcessor::setLdrComponentVal(DspFloatType ldrVal)
{
    // input is current off of diode from envelope follower, LDR curve is roughly logarithmic, so:
    // normalize --> take log --> multiply against resistor range
    
    // resistance values come from datasheet and Eichas & Zölzer paper
    const DspFloatType maxResistance = 1e6;
    const DspFloatType minResistance = 100;
    
    if(ldrVal > 1.f) ldrVal = 1.f;
    else if (ldrVal < 0.f) ldrVal = ldrVal * -1;
    
    // LDR curve looks roughly logarithmic. For easy of use, log function shifted to intercept (0,1) and (1,0)
    DspFloatType logVal = -log10(ldrVal * 1e3  + 0.1);
    
    // Clip
    if(logVal > 1.f) logVal = 1.f;
    else if (logVal < 0.f) logVal = 0.f;
    
    m_fLdrComponentVal = logVal * (maxResistance - minResistance) + minResistance;
}

DspFloatType wdfGainProcessor::getLdrComponentVal()
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
void wdfEnvelopeFollower::processSample(DspFloatType sampleIn, DspFloatType &sampleOut)
{
    m_pHpf->processSample(sampleIn, sampleIn);
    // need to get Thevenin equivalent
    m_pInputSource->Vs = sampleIn * m_fR1ComponentVal;
    cycleWave();
    sampleOut = (DspFloatType)m_pSeriesAdapters[kSeriesAdapterTreeLevel1]->upPort->getPortVoltage();
}

void wdfEnvelopeFollower::reset()
{
    m_pCap.get()->prevA = 0;
    m_pHpf->reset();
    m_pHpf->adaptTree();
    initTree();
}

void wdfEnvelopeFollower::setSampleRate(DspFloatType sampleRate)
{
    setSampleRate(sampleRate);
    m_pHpf->setSamplerate(sampleRate);
}

// Setters and Getters
void wdfEnvelopeFollower::setR1ComponentVal(DspFloatType r1Val, bool isNormalized)
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

void wdfEnvelopeFollower::setR2ComponentVal(DspFloatType r2Val, bool isNormalized)
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

void wdfEnvelopeFollower::setCapComponentVal(DspFloatType capVal, bool isNormalized)
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

DspFloatType wdfEnvelopeFollower::getR1ComponentVal(bool isNormalized)
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

DspFloatType wdfEnvelopeFollower::getR2ComponentVal(bool isNormalized)
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

DspFloatType wdfEnvelopeFollower::getCapComponentVal(bool isNormalized)
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

