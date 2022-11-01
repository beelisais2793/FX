#pragma once
//https://github.com/SMCFY/PolygonalOscillator/blob/master/Source/Oscillator.cpp

namespace Analog::Oscillators::PolygonalOscillator
{
    class Oscillator
    {
    public:

        Oscillator(int fs, int buffSize);
        ~Oscillator();

        void generatePolygon();
        void synthesizeWaveform(DspFloatType* buff);

        void updateFreq(const int& f0);
        void updateOrder(const int& n);
        void updateTeeth(const DspFloatType& t);
        void updatePhaseOffset(const DspFloatType& phaseOffset);
        void updateRadius(const DspFloatType& r);

        int getFreq();
        Range<int> getFreqLimits();
        int getOrder();
        DspFloatType getTeeth();
        DspFloatType getPhaseOffset();
        DspFloatType getRadius();
        int getBufferSize();
        Point<DspFloatType> getDrawCoords(const int& i); // returns the cartesian coordinates of the sampled geometry for rendering

    private:

        void polyBLAMP(DspFloatType* buff);

        int f0; // frequency
        DspFloatType n; // order
        DspFloatType t; // teeth
        DspFloatType phaseOffset; // phase offset in radians
        DspFloatType r; // radius (amplitude)

        int fs; // sampling rate
        int buffSize;
        DspFloatType pi;

        bool isClipped;
        DspFloatType pMax; // maximum radial amplitude
        dsp::Phase<DspFloatType> theta;

        DspFloatType* diff; // second order difference of the synthesized waveform
        DspFloatType diffBuff; // storing the last sample of the frame, for calculating difference
        DspFloatType disc; // location of discontinuity, expressed in samples

        DspFloatType* p; // vector of radial amplitude
        DspFloatType* pRender; // radial amplitude used for rendering
        std::complex<DspFloatType>* polygon; // sampled polygon

        // predefined ranges for oscillator parameters
        Range<int> freqRange;
        Range<int> orderRange;
        Range<DspFloatType> teethRange;
        Range<DspFloatType> phaseOffRange;
        Range<DspFloatType> radRange;


    };

    Oscillator::Oscillator(int fs, int buffSize)
    : pi(MathConstants<DspFloatType>::pi), f0(60), n(4), t(0.0f), phaseOffset(0.0f), r(0.9f), isClipped(false), pMax(0), diffBuff(0), disc(0),
    freqRange(Range<int>(60, 2000)), orderRange(Range<int>(3, 30)), teethRange(Range<DspFloatType>(0.0f, 0.4f)), phaseOffRange(Range<DspFloatType>(0.0f, MathConstants<DspFloatType>::twoPi)), radRange(Range<DspFloatType>(0.1f, 0.9f))
    {
        p = new DspFloatType[buffSize];
        pRender = new DspFloatType[buffSize];
        polygon = new std::complex<DspFloatType>[buffSize];
        diff = new DspFloatType[buffSize];
        
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

        dsp::Phase<DspFloatType> phi;
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

    void Oscillator::synthesizeWaveform(DspFloatType* buff)
    {   

        if(isClipped)
        {
            for(int i = 0; i < buffSize; i++)
            {
                p[i] = std::cos(pi/n) / std::cos(2*pi/n * fmod((theta.phase+phaseOffset)*n/(2*pi), 1) -pi/n + t) * r;  // radial amplitude
        
                buff[i] = p[i]/pMax*radRange.getEnd() * std::sin(theta.phase); // sample the y axis with normalized radial amplitude
        
                theta.advance(2*pi*DspFloatType(f0)*(1.0f/DspFloatType(fs)));  // increment phase
            }
        }
        else
        {
            for(int i = 0; i < buffSize; i++)
            {
                p[i] = std::cos(pi/n) / std::cos(2*pi/n * fmod((theta.phase+phaseOffset)*n/(2*pi), 1) -pi/n + t) * r;  // radial amplitude
        
                buff[i] = p[i] * std::sin(theta.phase); // sample the y axis
        
                theta.advance(2*pi*DspFloatType(f0)*(1.0f/DspFloatType(fs)));  // increment phase
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

    void Oscillator::updateTeeth(const DspFloatType& t)
    {
        this->t = teethRange.clipValue(t);
    }

    void Oscillator::updatePhaseOffset(const DspFloatType& phaseOffset)
    {
        this->phaseOffset = phaseOffRange.clipValue(phaseOffset);
    }

    void Oscillator::updateRadius(const DspFloatType& r)
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

    DspFloatType Oscillator::getTeeth()
    {
        return t;
    }

    DspFloatType Oscillator::getPhaseOffset()
    {
        return phaseOffset;
    }

    DspFloatType Oscillator::getRadius()
    {
        return r;
    }

    int Oscillator::getBufferSize()
    {
        return buffSize;
    }

    Point<DspFloatType> Oscillator::getDrawCoords(const int& i)
    {
        return Point<DspFloatType>(polygon[i].real(), polygon[i].imag());
    }

    //==============================================================================

    void Oscillator::polyBLAMP(DspFloatType* buff)
    {
        
        diff[0] = (diffBuff-buff[0]) - (buff[0]-buff[1]); // calculate first element from stored buffer (offset)
        for(int i = 1; i < buffSize; i++)
        {
            diff[i] = (buff[i-1]-buff[i]) - (buff[i]-buff[i+1]);
        }
        DspFloatType sampEstimate = buff[buffSize-1] + (buff[buffSize-1]-buff[buffSize-2]); // estimate future sample through linear regression 
        diff[buffSize-1] = (buff[buffSize-2]-buff[buffSize-1]) - (buff[buffSize-1]-sampEstimate); // last element is estimated

        diffBuff = buff[buffSize-1]; // update buffer with last sample from buffer


        int nDisc = ceil(buffSize/(DspFloatType(fs)/f0)*n); // number of discontinuities
        DspFloatType dDisc = DspFloatType(fs)/(n*f0); // interval between discontinuities
        
        for(int k = 0; k < nDisc; k++)
        {
            
            disc = TouchHandler::negMod(disc+dDisc - DspFloatType(fs)/f0/(2*pi/phaseOffset), buffSize); // location of discontinuities expressed in samples
            
            // boundary samples
            int n3 = ceil(disc);
            int n1 = TouchHandler::negMod(n3-2, buffSize);
            int n2 = TouchHandler::negMod(n1+1, buffSize);
            int n4 = TouchHandler::negMod(n3+1, buffSize);

            DspFloatType d = n3 - disc; // fractional delay between the discontinuity and the next sample

            DspFloatType u = abs(diff[n2]+ (disc-n2) * ((diff[n3]-diff[n2]) / (n3-n2)) ) *2; // slope of the derivative at the exact discontinuity

            // 4-point polyBLAMP residual coefficients
            DspFloatType p0 = pow(d,5)/120;
            DspFloatType p1 = (-3*pow(d,5) +5*pow(d,4) +10*pow(d,3) +10*pow(d,2) +5*d +1)/120;
            DspFloatType p2 = (3*pow(d,5) -10*pow(d,4) +40*pow(d,2) -60*d +28)/120;
            DspFloatType p3 = (-pow(d,5) +5*pow(d,4) -10*pow(d,3) +10*pow(d,2) -5*d +1)/120;

            // waveform correction on the four samples around the discontinuity
            buff[n1] -= p0*u * ((buff[n1] > 0) ? (1):(-1));
            buff[n2] -= p1*u * ((buff[n2] > 0) ? (1):(-1));
            buff[n3] -= p2*u * ((buff[n3] > 0) ? (1):(-1));
            buff[n4] -= p3*u * ((buff[n4] > 0) ? (1):(-1));

        }
    }
}