#pragma once
/*
Copyright (c) 2016, UMR STMS 9912 - Ircam-Centre Pompidou / CNRS / UPMC
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <cstring>


enum {
  LIMITER_OK = 0,

  __error_codes_start = -100,

  LIMITER_INVALID_HANDLE,
  LIMITER_INVALID_PARAMETER,

  __error_codes_end
};

#define PEAKLIMITER_ATTACK_DEFAULT_MS      (20.0f)               /* default attack  time in ms */
#define PEAKLIMITER_RELEASE_DEFAULT_MS     (20.0f)              /* default release time in ms */


class PeakLimiter
{

public:
  int  m_attack;
  DspFloatType         m_attackConst, m_releaseConst;
  DspFloatType         m_attackMs, m_releaseMs, m_maxAttackMs;
  DspFloatType         m_threshold;
  int  m_channels, m_maxChannels;
  int  m_sampleRate, m_maxSampleRate;
  DspFloatType         m_fadedGain;
  DspFloatType*        m_pMaxBuffer;
  DspFloatType*        m_pMaxBufferSlow;
  DspFloatType*        m_pDelayBuffer;
  int  m_maxBufferIndex, m_maxBufferSlowIndex, m_delayBufferIndex;
  int  m_sectionLen, m_nbrMaxBufferSection;
  int  m_maxBufferSectionIndex, m_maxBufferSectionCounter;
  DspFloatType        m_smoothState;
  DspFloatType         m_maxMaxBufferSlow, m_maxCurrentSection;
  int m_indexMaxBufferSlow, *m_pIndexMaxInSection;
    
public:

/******************************************************************************
* createLimiter                                                               *
* maxAttackMs:   maximum attack/lookahead time in milliseconds                *
* releaseMs:     release time in milliseconds (90% time constant)             *
* threshold:     limiting threshold                                           *
* maxChannels:   maximum number of channels                                   *
* maxSampleRate: maximum sampling rate in Hz                                  *
* returns:       limiter handle                                               *
******************************************************************************/
PeakLimiter(               DspFloatType         maxAttackMs, 
                           DspFloatType         releaseMs, 
                           DspFloatType         threshold, 
                           int  maxChannels, 
                           int  maxSampleRate);
~PeakLimiter();
/******************************************************************************
* resetLimiter                                                                *
* limiter: limiter handle                                                     *
* returns: error code                                                         *
******************************************************************************/
int resetLimiter();

/******************************************************************************
* destroyLimiter                                                              *
* limiter: limiter handle                                                     *
* returns: error code                                                         *
******************************************************************************/
int destroyLimiter();

/******************************************************************************
* applyLimiter                                                                *
* limiter:  limiter handle                                                    *
* samplesIn:  input buffer containing interleaved samples                *
* samplesOut:  output buffer containing interleaved samples                *
* nSamples: number of samples per channel                                     *
* returns:  error code                                                        *
******************************************************************************/
int applyLimiter_E( 
                 const DspFloatType*       samplesIn, 
                 DspFloatType*       samplesOut, 
                 int nSamples);

/******************************************************************************
* applyLimiter                                                                *
* limiter:  limiter handle                                                    *
* samplesIn:  input buffer containing no interleaved samples                *
* samplesOut:  output buffer containing interleaved samples                *
* nSamples: number of samples per channel                                     *
* returns:  error code                                                        *
******************************************************************************/
int applyLimiter(
                 const DspFloatType**       samplesIn, 
                 DspFloatType**       samplesOut, 
                 int nSamples);

/******************************************************************************
* applyLimiter                                                                *
* limiter:  limiter handle                                                    *
* samples:  input/output buffer containing no interleaved samples                *
* nSamples: number of samples per channel                                     *
* returns:  error code                                                        *
******************************************************************************/
int applyLimiter_I(
                 DspFloatType**       samples, 
                 int nSamples);

/******************************************************************************
* applyLimiter                                                                *
* limiter:  limiter handle                                                    *
* samples:  input/output buffer containing interleaved samples                *
* nSamples: number of samples per channel                                     *
* returns:  error code                                                        *
******************************************************************************/
int applyLimiter_E_I(
                 DspFloatType*       samples, 
                 int nSamples);

/******************************************************************************
* getLimiterDelay                                                             *
* limiter: limiter handle                                                     *
* returns: exact delay caused by the limiter in samples                       *
******************************************************************************/
 int getLimiterDelay();

 int getLimiterSampleRate();

DspFloatType getLimiterAttack();

DspFloatType getLimiterRelease();

DspFloatType getLimiterThreshold();

/******************************************************************************
* getLimiterMaxGainReduction                                                  *
* limiter: limiter handle                                                     *
* returns: maximum gain reduction in last processed block in dB               *
******************************************************************************/
DspFloatType getLimiterMaxGainReduction();

/******************************************************************************
* setLimiterNChannels                                                         *
* limiter:   limiter handle                                                   *
* nChannels: number of channels ( <= maxChannels specified on create)         *
* returns:   error code                                                       *
******************************************************************************/
int setLimiterNChannels( int nChannels);

/******************************************************************************
* setLimiterSampleRate                                                        *
* limiter:    limiter handle                                                  *
* sampleRate: sampling rate in Hz ( <= maxSampleRate specified on create)     *
* returns:    error code                                                      *
******************************************************************************/
int setLimiterSampleRate( int sampleRate);

/******************************************************************************
* setLimiterAttack                                                            *
* limiter:    limiter handle                                                  *
* attackMs:   attack time in ms ( <= maxAttackMs specified on create)         *
* returns:    error code                                                      *
******************************************************************************/
int setLimiterAttack( DspFloatType attackMs);

/******************************************************************************
* setLimiterRelease                                                           *
* limiter:    limiter handle                                                  *
* releaseMs:  release time in ms                                              *
* returns:    error code                                                      *
******************************************************************************/
int setLimiterRelease( DspFloatType releaseMs);

/******************************************************************************
* setLimiterThreshold                                                         *
* limiter:    limiter handle                                                  *
* threshold:  limiter threshold                                               *
* returns:    error code                                                      *
******************************************************************************/
int setLimiterThreshold( DspFloatType threshold);
};


#ifndef max
#define max(a, b)   (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b)   (((a) < (b)) ? (a) : (b))
#endif

/* create limiter */
 PeakLimiter::PeakLimiter(
                           DspFloatType         maxAttackMsIn,
                           DspFloatType         releaseMsIn,
                           DspFloatType         thresholdIn,
                           int  maxChannelsIn,
                           int  maxSampleRateIn
                           )
{

  /* calc m_attack time in samples */
  m_attack = (int)(maxAttackMsIn * maxSampleRateIn / 1000);

  if (m_attack < 1) /* m_attack time is too short */
	  m_attack = 1; 

  /* length of m_pMaxBuffer sections */
  m_sectionLen = (int)sqrt((DspFloatType)m_attack+1);
  /* sqrt(m_attack+1) leads to the minimum 
     of the number of maximum operators:
     nMaxOp = m_sectionLen + (m_attack+1)/m_sectionLen */

  /* alloc limiter struct */

  m_nbrMaxBufferSection = (m_attack+1)/m_sectionLen;
  if (m_nbrMaxBufferSection*m_sectionLen < (m_attack+1))
    m_nbrMaxBufferSection++; /* create a full section for the last samples */

  /* alloc maximum and delay buffers */
  m_pMaxBuffer   = new DspFloatType[m_nbrMaxBufferSection * m_sectionLen];
  m_pDelayBuffer   = new DspFloatType[m_attack * maxChannelsIn];
  m_pMaxBufferSlow   = new DspFloatType[m_nbrMaxBufferSection];
  m_pIndexMaxInSection = new  int[m_nbrMaxBufferSection];

  if ((m_pMaxBuffer==NULL) || (m_pDelayBuffer==NULL) || (m_pMaxBufferSlow==NULL)) {
    destroyLimiter();
    return;
  }

  /* init parameters & states */
  m_maxBufferIndex = 0;
  m_delayBufferIndex = 0;
  m_maxBufferSlowIndex = 0;
  m_maxBufferSectionIndex = 0;
  m_maxBufferSectionCounter = 0;
  m_maxMaxBufferSlow = 0;
  m_indexMaxBufferSlow = 0;
  m_maxCurrentSection = 0;

  m_attackMs      = maxAttackMsIn;
  m_maxAttackMs   = maxAttackMsIn;
  m_attackConst   = (DspFloatType)pow(0.1, 1.0 / (m_attack + 1));
  m_releaseConst  = (DspFloatType)pow(0.1, 1.0 / (m_releaseMs * maxSampleRateIn / 1000 + 1));
  m_threshold     = thresholdIn;
  m_channels      = maxChannelsIn;
  m_maxChannels   = maxChannelsIn;
  m_sampleRate    = maxSampleRateIn;
  m_maxSampleRate = maxSampleRateIn;
    

  m_fadedGain = 1.0f;
  m_smoothState = 1.0;
    
    memset(m_pMaxBuffer,0,sizeof(DspFloatType)*m_nbrMaxBufferSection * m_sectionLen);
    memset(m_pDelayBuffer,0,sizeof(DspFloatType)*m_attack * maxChannelsIn);
    memset(m_pMaxBufferSlow,0,sizeof(DspFloatType)*m_nbrMaxBufferSection);
    memset(m_pIndexMaxInSection,0,sizeof( int)*m_nbrMaxBufferSection);
}

PeakLimiter::~PeakLimiter()
{
    destroyLimiter();
}

/* reset limiter */
int PeakLimiter::resetLimiter()
{
 
    m_maxBufferIndex = 0;
    m_delayBufferIndex = 0;
    m_maxBufferSlowIndex = 0;
    m_maxBufferSectionIndex = 0;
    m_maxBufferSectionCounter = 0;
    m_fadedGain = 1.0f;
    m_smoothState = 1.0;
    m_maxMaxBufferSlow = 0;
    m_indexMaxBufferSlow = 0;
    m_maxCurrentSection = 0;


    memset(m_pMaxBuffer,0,sizeof(DspFloatType)*m_nbrMaxBufferSection * m_sectionLen);
    memset(m_pDelayBuffer,0,sizeof(DspFloatType)*m_attack * m_maxChannels);
    memset(m_pMaxBufferSlow,0,sizeof(DspFloatType)*m_nbrMaxBufferSection);
    memset(m_pIndexMaxInSection,0,sizeof(int)*m_nbrMaxBufferSection);
  
  return LIMITER_OK;
}


/* destroy limiter */
int PeakLimiter::destroyLimiter()
{
    if (m_pMaxBuffer)
    {
        delete [] m_pMaxBuffer;
        m_pMaxBuffer = NULL;
    }
    if (m_pDelayBuffer)
    {
     delete [] m_pDelayBuffer;
        m_pDelayBuffer = NULL;
    }
    if (m_pMaxBufferSlow)
    {
        delete [] m_pMaxBufferSlow;
        m_pMaxBufferSlow = NULL;
    }
    if (m_pIndexMaxInSection)
    {
        delete [] m_pIndexMaxInSection;
        m_pIndexMaxInSection = NULL;
    }
    
    return LIMITER_OK;
}

/* apply limiter */
int PeakLimiter::applyLimiter_E(const DspFloatType *samplesIn,DspFloatType *samplesOut, int nSamples)
{
	memcpy(samplesOut,samplesIn,nSamples*sizeof(DspFloatType));
	return applyLimiter_E_I(samplesOut,nSamples);
}

/* apply limiter */
int PeakLimiter::applyLimiter_E_I(DspFloatType *samples, int nSamples)
{
   int i, j;
    DspFloatType tmp, gain, maximum;

    for (i = 0; i < nSamples; i++) {
        /* get maximum absolute sample value of all channels that are greater in absoulte value to m_threshold */
        m_pMaxBuffer[m_maxBufferIndex] = m_threshold;
        for (j = 0; j < m_channels; j++) {
            m_pMaxBuffer[m_maxBufferIndex] = max(m_pMaxBuffer[m_maxBufferIndex], (DspFloatType)fabs(samples[i * m_channels + j]));
        }

        /* search maximum in the current section */
        if (m_pIndexMaxInSection[m_maxBufferSlowIndex] == m_maxBufferIndex) // if we have just changed the sample containg the old maximum value
        {
            // need to compute the maximum on the whole section 
            m_maxCurrentSection = m_pMaxBuffer[m_maxBufferSectionIndex];
            for (j = 1; j < m_sectionLen; j++) {
                if (m_pMaxBuffer[m_maxBufferSectionIndex + j] > m_maxCurrentSection)
                {
                    m_maxCurrentSection = m_pMaxBuffer[m_maxBufferSectionIndex + j];
                    m_pIndexMaxInSection[m_maxBufferSlowIndex] = m_maxBufferSectionIndex + j;
                }
            }
        }
        else // just need to compare the new value the cthe current maximum value
        {
            if (m_pMaxBuffer[m_maxBufferIndex] > m_maxCurrentSection)
            {
                m_maxCurrentSection = m_pMaxBuffer[m_maxBufferIndex];
                m_pIndexMaxInSection[m_maxBufferSlowIndex] = m_maxBufferIndex;
            }
        }

        // find maximum of slow (downsampled) max buffer
        maximum = m_maxMaxBufferSlow;
        if (m_maxCurrentSection > maximum)
        {
            maximum = m_maxCurrentSection;
        }

        m_maxBufferIndex++;
        m_maxBufferSectionCounter++;

        /* if m_pMaxBuffer section is finished, or end of m_pMaxBuffer is reached,
        store the maximum of this section and open up a new one */
        if ((m_maxBufferSectionCounter >= m_sectionLen) || (m_maxBufferIndex >= m_attack + 1)) {
            m_maxBufferSectionCounter = 0;

            tmp = m_pMaxBufferSlow[m_maxBufferSlowIndex] = m_maxCurrentSection;
            j = 0;
            if (m_indexMaxBufferSlow == m_maxBufferSlowIndex)
            {
                j = 1;
            }
            m_maxBufferSlowIndex++;
            if (m_maxBufferSlowIndex >= m_nbrMaxBufferSection)
            {
                m_maxBufferSlowIndex = 0;
            }
            if (m_indexMaxBufferSlow == m_maxBufferSlowIndex)
            {
                j = 1;
            }
            m_maxCurrentSection = m_pMaxBufferSlow[m_maxBufferSlowIndex];
            m_pMaxBufferSlow[m_maxBufferSlowIndex] = 0.0f;  /* zero out the value representing the new section */

            /* compute the maximum over all the section */
            if (j)
            {
                m_maxMaxBufferSlow = 0;
                for (j = 0; j < m_nbrMaxBufferSection; j++)
                {
                    if (m_pMaxBufferSlow[j] > m_maxMaxBufferSlow)
                    {
                        m_maxMaxBufferSlow = m_pMaxBufferSlow[j];
                        m_indexMaxBufferSlow = j;
                    }
                }
            }
            else
            {
                if (tmp > m_maxMaxBufferSlow)
                {
                    m_maxMaxBufferSlow = tmp;
                    m_indexMaxBufferSlow = m_maxBufferSlowIndex;
                }
            }

            m_maxBufferSectionIndex += m_sectionLen;
        }

        if (m_maxBufferIndex >= (m_attack + 1))
        {
            m_maxBufferIndex = 0;
            m_maxBufferSectionIndex = 0;
        }

        /* needed current gain */
        if (maximum > m_threshold)
        {
            gain = m_threshold / maximum;
        }
        else
        {
            gain = 1;
        }

        /*avoid overshoot */

        if (gain < m_smoothState) {
            m_fadedGain = min(m_fadedGain, (gain - 0.1f * (DspFloatType)m_smoothState) * 1.11111111f);
        }
        else
        {
            m_fadedGain = gain;
        }


        /* smoothing gain */
        if (m_fadedGain < m_smoothState)
        {
            m_smoothState = m_attackConst * (m_smoothState - m_fadedGain) + m_fadedGain;  /* m_attack */
            /*avoid overshoot */
            if (gain > m_smoothState)
            {
                m_smoothState = gain;
            }
        }
        else
        {
            m_smoothState = m_releaseConst * (m_smoothState - m_fadedGain) + m_fadedGain; /* release */
        }

        /* fill delay line, apply gain */
        for (j = 0; j < m_channels; j++)
        {
            tmp = m_pDelayBuffer[m_delayBufferIndex * m_channels + j];
            m_pDelayBuffer[m_delayBufferIndex * m_channels + j] = samples[i * m_channels + j];

            tmp *= m_smoothState;
            if (tmp > m_threshold) tmp = m_threshold;
            if (tmp < -m_threshold) tmp = -m_threshold;

            samples[i * m_channels + j] = tmp;
        }

        m_delayBufferIndex++;
        if (m_delayBufferIndex >= m_attack)
            m_delayBufferIndex = 0;

    }

    return LIMITER_OK;
}

/* apply limiter */
int PeakLimiter::applyLimiter(const DspFloatType **samplesIn,DspFloatType **samplesOut, int nSamples)
{
	int ind;
	for(ind=0;ind<m_channels;ind++)
	{
		memcpy(samplesOut[ind],samplesIn[ind],nSamples*sizeof(DspFloatType));
	}
	return applyLimiter_I(samplesOut,nSamples);
}


/* apply limiter */
int PeakLimiter::applyLimiter_I( DspFloatType **samples,int nSamples)
{
   int i, j;
    DspFloatType tmp, gain, maximum;
    
    for (i = 0; i < nSamples; i++) {
        /* get maximum absolute sample value of all channels that are greater in absoulte value to m_threshold */
        m_pMaxBuffer[m_maxBufferIndex] = m_threshold;
        for (j = 0; j < m_channels; j++) {
            m_pMaxBuffer[m_maxBufferIndex] = max(m_pMaxBuffer[m_maxBufferIndex], (DspFloatType)fabs(samples[j][i]));
        }
                
        /* search maximum in the current section */
        if (m_pIndexMaxInSection[m_maxBufferSlowIndex] == m_maxBufferIndex) // if we have just changed the sample containg the old maximum value
        {
            // need to compute the maximum on the whole section 
            m_maxCurrentSection = m_pMaxBuffer[m_maxBufferSectionIndex];
            for (j = 1; j < m_sectionLen; j++) {
                if (m_pMaxBuffer[m_maxBufferSectionIndex + j] > m_maxCurrentSection)
                {
                    m_maxCurrentSection = m_pMaxBuffer[m_maxBufferSectionIndex + j];
                    m_pIndexMaxInSection[m_maxBufferSlowIndex] = m_maxBufferSectionIndex+j;
                }
            }
        }
        else // just need to compare the new value the cthe current maximum value
        {
            if (m_pMaxBuffer[m_maxBufferIndex] > m_maxCurrentSection)
            {
                m_maxCurrentSection = m_pMaxBuffer[m_maxBufferIndex];
                m_pIndexMaxInSection[m_maxBufferSlowIndex] = m_maxBufferIndex;
            }
        }
        
        // find maximum of slow (downsampled) max buffer
        maximum = m_maxMaxBufferSlow;
        if (m_maxCurrentSection > maximum)
        {
            maximum = m_maxCurrentSection;
        }
    
        m_maxBufferIndex++;
        m_maxBufferSectionCounter++;
        
        /* if m_pMaxBuffer section is finished, or end of m_pMaxBuffer is reached,
         store the maximum of this section and open up a new one */
        if ((m_maxBufferSectionCounter >= m_sectionLen)||(m_maxBufferIndex >= m_attack+1)) {
            m_maxBufferSectionCounter = 0;
            
            tmp = m_pMaxBufferSlow[m_maxBufferSlowIndex] = m_maxCurrentSection;
            j = 0;
            if (m_indexMaxBufferSlow == m_maxBufferSlowIndex)
            {
                j = 1;
            }
            m_maxBufferSlowIndex++;
            if (m_maxBufferSlowIndex >= m_nbrMaxBufferSection)
            {
                m_maxBufferSlowIndex = 0;
            }
            if (m_indexMaxBufferSlow == m_maxBufferSlowIndex)
            {
                j = 1;
            }
            m_maxCurrentSection = m_pMaxBufferSlow[m_maxBufferSlowIndex];
            m_pMaxBufferSlow[m_maxBufferSlowIndex] = 0.0f;  /* zero out the value representing the new section */

            /* compute the maximum over all the section */
            if (j)
            {
                m_maxMaxBufferSlow = 0;
                for (j = 0; j < m_nbrMaxBufferSection; j++)
                {
                    if (m_pMaxBufferSlow[j] > m_maxMaxBufferSlow)
                    {
                        m_maxMaxBufferSlow = m_pMaxBufferSlow[j];
                        m_indexMaxBufferSlow = j;
                    }
                }
            }
            else
            {
                if (tmp > m_maxMaxBufferSlow)
                {
                    m_maxMaxBufferSlow = tmp;
                    m_indexMaxBufferSlow = m_maxBufferSlowIndex;
                }
            }
            
            m_maxBufferSectionIndex += m_sectionLen;
        }
        
        if (m_maxBufferIndex >= (m_attack+1))
        {
            m_maxBufferIndex = 0;
            m_maxBufferSectionIndex = 0;
        }
        
        /* needed current gain */
        if (maximum > m_threshold)
        {
            gain = m_threshold / maximum;
        }
        else
        {
            gain = 1;
        }
        
        /*avoid overshoot */
 
        if (gain < m_smoothState) {
            m_fadedGain = min(m_fadedGain, (gain - 0.1f * (DspFloatType)m_smoothState) * 1.11111111f);
        }
        else 
        {
            m_fadedGain = gain;
        }


        /* smoothing gain */
        if (m_fadedGain < m_smoothState)
        {
            m_smoothState = m_attackConst * (m_smoothState - m_fadedGain) + m_fadedGain;  /* m_attack */
            /*avoid overshoot */
            if (gain > m_smoothState)
            {
                m_smoothState = gain;
            }
        }
        else
        {
            m_smoothState = m_releaseConst * (m_smoothState - m_fadedGain) + m_fadedGain; /* release */
        }
        
        /* fill delay line, apply gain */
        for (j = 0; j < m_channels; j++)
        {
            tmp = m_pDelayBuffer[m_delayBufferIndex * m_channels + j];
            m_pDelayBuffer[m_delayBufferIndex * m_channels + j] = samples[j][i];
            
            tmp *= m_smoothState;
            if (tmp > m_threshold) tmp = m_threshold;
            if (tmp < -m_threshold) tmp = -m_threshold;
            
            samples[j][i] = tmp;
        }
        
        m_delayBufferIndex++;
        if (m_delayBufferIndex >= m_attack)
            m_delayBufferIndex = 0;
        
    }
    
    return LIMITER_OK;
    
}

/* get delay in samples */
int PeakLimiter::getLimiterDelay()
{
  return m_attack;
}

/* get m_attack in Ms */
DspFloatType PeakLimiter::getLimiterAttack()
{
  return m_attackMs;
}

/* get delay in samples */
int PeakLimiter::getLimiterSampleRate()
{
	return m_sampleRate;
}

/* get delay in samples */
DspFloatType PeakLimiter::getLimiterRelease()
{
	return m_releaseMs;
}
 
/* get maximum gain reduction of last processed block */
DspFloatType PeakLimiter::getLimiterMaxGainReduction()
{
  return -20 * (DspFloatType)log10(m_smoothState);
}

/* set number of channels */
int PeakLimiter::setLimiterNChannels(int nChannelsIn)
{
  if (nChannelsIn > m_maxChannels) return LIMITER_INVALID_PARAMETER;

  m_channels = nChannelsIn;
  resetLimiter();

  return LIMITER_OK;
}

/* set sampling rate */
int PeakLimiter::setLimiterSampleRate(int sampleRateIn)
{
  if (sampleRateIn > m_maxSampleRate) return LIMITER_INVALID_PARAMETER;

  /* update m_attack/release constants */
  m_attack = (int)(m_attackMs * sampleRateIn / 1000);

  if (m_attack < 1) /* m_attack time is too short */
    return LIMITER_INVALID_PARAMETER; 

  /* length of m_pMaxBuffer sections */
  m_sectionLen = (int)sqrt((DspFloatType)m_attack+1);

  m_nbrMaxBufferSection    = (m_attack+1)/m_sectionLen;
  if (m_nbrMaxBufferSection*m_sectionLen < (m_attack+1))
    m_nbrMaxBufferSection++;
  m_attackConst   = (DspFloatType)pow(0.1, 1.0 / (m_attack + 1));
  m_releaseConst  = (DspFloatType)pow(0.1, 1.0 / (m_releaseMs * sampleRateIn / 1000 + 1));
  m_sampleRate    = sampleRateIn;

  /* reset */
  resetLimiter();

  return LIMITER_OK;
}

/* set m_attack time */
int PeakLimiter::setLimiterAttack(DspFloatType attackMsIn)
{
  if (attackMsIn > m_maxAttackMs) return LIMITER_INVALID_PARAMETER;

  /* calculate attack time in samples */
  m_attack = (int)(attackMsIn * m_sampleRate / 1000);

  if (m_attack < 1) /* attack time is too short */
    m_attack=1;

  /* length of m_pMaxBuffer sections */
  m_sectionLen = (int)sqrt((DspFloatType)m_attack+1);

  m_nbrMaxBufferSection   = (m_attack+1)/m_sectionLen;
  if (m_nbrMaxBufferSection*m_sectionLen < (m_attack+1))
    m_nbrMaxBufferSection++;
  m_attackConst  = (DspFloatType)pow(0.1, 1.0 / (m_attack + 1));
  m_attackMs     = attackMsIn;

  /* reset */
  resetLimiter();

  return LIMITER_OK;
}

/* set release time */
int PeakLimiter::setLimiterRelease(DspFloatType releaseMsIn)
{ 
  m_releaseConst = (DspFloatType)pow(0.1, 1.0 / (releaseMsIn * m_sampleRate / 1000 + 1));
  m_releaseMs = releaseMsIn;

  return LIMITER_OK;
}

/* set limiter threshold */
int PeakLimiter::setLimiterThreshold(DspFloatType thresholdIn)
{
  m_threshold = thresholdIn;

  return LIMITER_OK;
}

/* set limiter threshold */
DspFloatType PeakLimiter::getLimiterThreshold()
{
  return m_threshold;
}
