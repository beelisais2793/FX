

#include "Envelope.h"

Envelope::Envelope()
: aMin(0.001f), trig(0), noteOn(0), amplitude(1), fs(44100), attack(0), decay(0), release(0), envelopeType(Envelope::env::ADSR),
attackTime(200), peak(0.95), decayTime(200), sustainLevel(0.8), releaseTime(1000)
{

}

Envelope::Envelope(Envelope::env type)
: aMin(0.001f), trig(0), noteOn(0), amplitude(1), fs(44100), attack(0), decay(0), release(0),
attackTime(200), peak(0.95), decayTime(200), sustainLevel(0.8), releaseTime(1000)
{
	envelopeType = type;
}

Envelope::~Envelope()
{

}

//=============================================================================

void Envelope::trigger()
{
	trig = true;

	if(!trig) // note off
		noteOn = 0;
}

float Envelope::envelopeAR() // AR
{
    relDelta = pow(aMin, peak / std::round(fs * (releaseTime/1000.0f))); // exponential release
    
    if(trig && !noteOn) // init on trigger/re-trigger
	{
 		attack = 1;
    	release = 1;

    	trig = 0;

    	attDelta = peak / std::round(fs * (attackTime/1000.0f)); // linear attack
   	}

	if(attack) // attack phase
	{
        amplitude += attDelta;
        if(amplitude >= peak)
        {
            amplitude = peak;
            attack = 0;
        }
		return amplitude;
	}
	else if(release) // release phase
	{
		if(amplitude >= aMin)
		{
			amplitude *= relDelta;
			return amplitude;
		}
		else
		{
			amplitude = 0;
			release = 0;
			return 0.0f;
		}
	}
	
    return 0.0f;
}

float Envelope::envelopeADSR() // ADSR
{
    relDelta = pow(aMin, sustainLevel / std::round(fs * (releaseTime/1000.0f))); // exponential release

	if(trig && !noteOn) // init on trigger/re-trigger
	{
		attack = 1;
		decay = 1;
		noteOn = 1; // sustain
    	release = 1;
	    
    	attDelta = peak / std::round(fs * (attackTime/1000.0f)); // linear attack
    	decDelta = pow(aMin, peak / std::round(fs * (decayTime/1000.0f))); // exponential decay
   	}

	if(attack) // attack phase
	{
		amplitude += attDelta;
		if (amplitude >= peak)
		{
            amplitude = peak;
			attack = 0;
		}
		return amplitude;
	}
	else if(decay && amplitude > sustainLevel) // decay phase
	{
		amplitude *= decDelta;
		if (attack <= sustainLevel)
		{
			amplitude = sustainLevel;
			decay = 0;
		}
		return amplitude;
	}
	else if(noteOn) // sustain phase
	{
		amplitude = sustainLevel;
		return amplitude;
	}
	else if(release) // release phase
	{
		amplitude *= relDelta;
		if(amplitude <= aMin)
		{
			amplitude = 0;
			release = 0;		
		}
		return amplitude;
	}

    return 0.0f;
}

//=============================================================================

void Envelope::process(AudioBuffer<float>& buffer)
{
    
	float **outputFrame = buffer.getArrayOfWritePointers();

	for (int samp = 0; samp < buffer.getNumSamples(); ++samp)
	{
	    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
	    {
	    	switch (envelopeType)
	    	{
            case AR:
                    outputFrame[ch][samp] *= envelopeAR();
	        break;
                    
            case ADSR:
                    outputFrame[ch][samp] *= envelopeADSR();
	        break;
            };
	    }
	}
	
}

//=============================================================================

void Envelope::setAttackTime(const int& a)
{
	attackTime = a;
}

void Envelope::setPeak(const float& p)
{
	peak = p;
}

void Envelope::setDecayTime(const int& d)
{
	decayTime = d;
}

void Envelope::setSustainLevel(const float& s)
{
	sustainLevel = s;
}

void Envelope::setReleaseTime(const int& r)
{
	releaseTime = r;
}

void Envelope::setSamplingRate(const int& fs)
{
	this->fs = fs;
}

//=============================================================================

float* Envelope::getAmplitude()
{
    return &amplitude;
}
