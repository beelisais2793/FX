

#include "Oscillator.h"

Oscillator::Oscillator(int fs, int buffSize)
: pi(MathConstants<float>::pi), f0(60), n(4), t(0.0f), phaseOffset(0.0f), r(0.9f), isClipped(false), pMax(0), diffBuff(0), disc(0),
freqRange(Range<int>(60, 2000)), orderRange(Range<int>(3, 30)), teethRange(Range<float>(0.0f, 0.4f)), phaseOffRange(Range<float>(0.0f, MathConstants<float>::twoPi)), radRange(Range<float>(0.1f, 0.9f))
{
    p = new float[buffSize];
    pRender = new float[buffSize];
    polygon = new std::complex<float>[buffSize];
    diff = new float[buffSize];
    
    this->fs = fs;
    this->buffSize = buffSize;
    
    theta.reset();

	generatePolygon();
}

Oscillator::~Oscillator()
{
    delete p;
    delete pRender;
	delete polygon;
    delete diff;
}

//==============================================================================

void Oscillator::generatePolygon()
{

    dsp::Phase<float> phi;
    isClipped = false;
    pMax = 0;

    for(int i=0; i<buffSize; i++)
    {
        pRender[i] = std::cos(pi/n) / std::cos(fmod(phi.phase+phaseOffset, 2*pi/n) -pi/n + t) * r;
        
        phi.advance(2*pi/buffSize);

        if(pRender[i] > radRange.getEnd()) // checks for clipping (radial amplitude out of range)
        {
            isClipped = true;
            pMax = jmax(pRender[i], pMax); // store maximum clipping radial amplitude
        }
    }

    if(isClipped) // normalize polygon if clipped
    {
        for (int i = 0; i < buffSize; i++)
            pRender[i] = pRender[i]/pMax*radRange.getEnd(); // normalize and cap to range
    }


    phi.reset();
    for(int i=0; i<buffSize; i++) // sampling
    {
        polygon[i].real(pRender[i] * std::cos(phi.phase));
        polygon[i].imag(pRender[i] * std::sin(phi.phase));

        phi.advance(2*pi/buffSize);
    }

}

void Oscillator::synthesizeWaveform(float* buff)
{   

    if(isClipped)
    {
        for(int i = 0; i < buffSize; i++)
        {
            p[i] = std::cos(pi/n) / std::cos(2*pi/n * fmod((theta.phase+phaseOffset)*n/(2*pi), 1) -pi/n + t) * r;  // radial amplitude
    
            buff[i] = p[i]/pMax*radRange.getEnd() * std::sin(theta.phase); // sample the y axis with normalized radial amplitude
    
            theta.advance(2*pi*float(f0)*(1.0f/float(fs)));  // increment phase
        }
    }
    else
    {
        for(int i = 0; i < buffSize; i++)
        {
            p[i] = std::cos(pi/n) / std::cos(2*pi/n * fmod((theta.phase+phaseOffset)*n/(2*pi), 1) -pi/n + t) * r;  // radial amplitude
    
            buff[i] = p[i] * std::sin(theta.phase); // sample the y axis
    
            theta.advance(2*pi*float(f0)*(1.0f/float(fs)));  // increment phase
        }
    }

    if(t == 0) // the teeth parameter doubles the number of discontinuities, which is not supported by the current implementation of polyBLAMP
        polyBLAMP(buff); // apply polyBLAMP anti-aliasing

}

//==============================================================================

void Oscillator::updateFreq(const int& f0)
{
	this->f0 = freqRange.clipValue(f0);
}

void Oscillator::updateOrder(const int& n)
{
	this->n = orderRange.clipValue(n);
}

void Oscillator::updateTeeth(const float& t)
{
	this->t = teethRange.clipValue(t);
}

void Oscillator::updatePhaseOffset(const float& phaseOffset)
{
	this->phaseOffset = phaseOffRange.clipValue(phaseOffset);
}

void Oscillator::updateRadius(const float& r)
{	
	this->r = radRange.clipValue(r);
}

//==============================================================================

int Oscillator::getFreq()
{
    return f0;
}

Range<int> Oscillator::getFreqLimits()
{
    return freqRange;
}

int Oscillator::getOrder()
{
    return n;
}

float Oscillator::getTeeth()
{
    return t;
}

float Oscillator::getPhaseOffset()
{
    return phaseOffset;
}

float Oscillator::getRadius()
{
    return r;
}

int Oscillator::getBufferSize()
{
    return buffSize;
}

Point<float> Oscillator::getDrawCoords(const int& i)
{
	return Point<float>(polygon[i].real(), polygon[i].imag());
}

//==============================================================================

void Oscillator::polyBLAMP(float* buff)
{
    
    diff[0] = (diffBuff-buff[0]) - (buff[0]-buff[1]); // calculate first element from stored buffer (offset)
    for(int i = 1; i < buffSize; i++)
    {
        diff[i] = (buff[i-1]-buff[i]) - (buff[i]-buff[i+1]);
    }
    float sampEstimate = buff[buffSize-1] + (buff[buffSize-1]-buff[buffSize-2]); // estimate future sample through linear regression 
    diff[buffSize-1] = (buff[buffSize-2]-buff[buffSize-1]) - (buff[buffSize-1]-sampEstimate); // last element is estimated

    diffBuff = buff[buffSize-1]; // update buffer with last sample from buffer


    int nDisc = ceil(buffSize/(float(fs)/f0)*n); // number of discontinuities
    float dDisc = float(fs)/(n*f0); // interval between discontinuities
    
    for(int k = 0; k < nDisc; k++)
    {
        
        disc = TouchHandler::negMod(disc+dDisc - float(fs)/f0/(2*pi/phaseOffset), buffSize); // location of discontinuities expressed in samples
        
        // boundary samples
        int n3 = ceil(disc);
        int n1 = TouchHandler::negMod(n3-2, buffSize);
        int n2 = TouchHandler::negMod(n1+1, buffSize);
        int n4 = TouchHandler::negMod(n3+1, buffSize);

        float d = n3 - disc; // fractional delay between the discontinuity and the next sample

        float u = abs(diff[n2]+ (disc-n2) * ((diff[n3]-diff[n2]) / (n3-n2)) ) *2; // slope of the derivative at the exact discontinuity

        // 4-point polyBLAMP residual coefficients
        float p0 = pow(d,5)/120;
        float p1 = (-3*pow(d,5) +5*pow(d,4) +10*pow(d,3) +10*pow(d,2) +5*d +1)/120;
        float p2 = (3*pow(d,5) -10*pow(d,4) +40*pow(d,2) -60*d +28)/120;
        float p3 = (-pow(d,5) +5*pow(d,4) -10*pow(d,3) +10*pow(d,2) -5*d +1)/120;

        // waveform correction on the four samples around the discontinuity
        buff[n1] -= p0*u * ((buff[n1] > 0) ? (1):(-1));
        buff[n2] -= p1*u * ((buff[n2] > 0) ? (1):(-1));
        buff[n3] -= p2*u * ((buff[n3] > 0) ? (1):(-1));
        buff[n4] -= p3*u * ((buff[n4] > 0) ? (1):(-1));

    }
}
