#pragma once
#include <cstdlib>
#include <cstring>

namespace Flanger
{

    class FractionalDelay
    {
    public:
        
        FractionalDelay();
        void setBufferSize(int size);
        
        void addSampleToBuffer(DspFloatType data);
        
        DspFloatType getSample(DspFloatType delay);
        
    private:
        
        void clear();
        
        void getreadOffset();
        DspFloatType interpolate();
        
        DspFloatType readOffset = 0;
        DspFloatType offset;
        
        int writePos = 0;
        int readPos = 0;
        
        DspFloatType outputSample;
        DspFloatType nextSample;
        
        int bufferSize;
        DspFloatType frac;
        DspFloatType* buffer;
        
    };

    inline FractionalDelay::FractionalDelay()
    {
        bufferSize = 0;
    }

    inline void FractionalDelay::setBufferSize(int size)
    {
        bufferSize = size;
        buffer = new DspFloatType[bufferSize];
        clear();
    }

    inline void FractionalDelay::clear()
    {
        std::memset(buffer, 0, bufferSize);
    }

    inline void FractionalDelay::addSampleToBuffer(DspFloatType data)
    {
        buffer[writePos] = data;
        writePos = (writePos != bufferSize - 1 ? writePos + 1 : 0);
    }


    inline DspFloatType FractionalDelay::getSample(DspFloatType delay)
    {
        offset = delay;
        getreadOffset();
        
        return interpolate();
    }


    inline void FractionalDelay::getreadOffset()
    {
        readOffset = writePos - offset;
        readOffset = (readOffset >= 0 ? (readOffset < bufferSize ? readOffset : readOffset - bufferSize)
                    : readOffset + bufferSize);
    }

    inline DspFloatType FractionalDelay::interpolate()
    {
        readPos = (int) readOffset;
        frac = readOffset - readPos;
        nextSample = (readPos != bufferSize - 1 ? buffer[readPos+1] : buffer[0]);
        return outputSample = buffer[readPos] + frac * (nextSample - buffer[readPos]);
    }


    class ToneGenerator
    {
    public:
        
        ToneGenerator();
        
        enum Waveshape {SINE, SAW, SQUARE, TRIANGLE};
        
        void initialiseWave(DspFloatType frequency, DspFloatType amplitude, DspFloatType sr, Waveshape wave);
        
        DspFloatType getValue();
        
    private:
        
        DspFloatType sineWave();
        DspFloatType sawWave();
        DspFloatType sqrWave();
        DspFloatType triWave();
        
        Waveshape kWave;
        DspFloatType kFrequency;
        DspFloatType kAmplitude;
        DspFloatType kSR;
        
        DspFloatType m;
        DspFloatType n;
        int tick = 0;
        DspFloatType output;
        DspFloatType currentAngle = 0;
    };


    inline ToneGenerator::ToneGenerator()
    {
        output = 0;
    }

    inline void ToneGenerator::initialiseWave(DspFloatType frequency, DspFloatType amplitude, DspFloatType sr, Waveshape wave)
    {
        kFrequency = frequency;
        kAmplitude = amplitude;
        kSR = sr;
        kWave = wave;
        if (kAmplitude > 1)
            kAmplitude = 1;
    }

    inline DspFloatType ToneGenerator::getValue()
    {
        if(kWave == SINE)
            return sineWave();
        if(kWave == SAW)
            return sawWave();
        if(kWave == SQUARE)
            return sqrWave();
        if(kWave == TRIANGLE)
            return triWave();
        
        else return 0;
    }

    inline DspFloatType ToneGenerator::sineWave()
    {
        DspFloatType delta = (kFrequency / kSR) * 2 * M_PI;
        currentAngle += delta;
        output = std::sin(currentAngle);
        return output;
    }

    inline DspFloatType ToneGenerator::sawWave()
    {
        m = kAmplitude / (kSR / kFrequency);
        
        if(output >= kAmplitude)
            output = - kAmplitude;
        
        output += m;
        
        return output;
    }

    inline DspFloatType ToneGenerator::sqrWave()
    {
        m  = (int) (kSR / (kFrequency));
        
        if(tick < (m / 2))
        {
            output = kAmplitude;
        }
        else
        {
            output = -kAmplitude;
        }
        
        if(tick >= m)
            tick = 0;
        else tick += 1;
        
        return output;
    }

    inline DspFloatType ToneGenerator::triWave()
    {
        m = kAmplitude / ((kSR / kFrequency) / 4);
        n = (int) (kSR / kFrequency);
        
        if(tick < (n / 4) || tick > ((3 * n) / 4))
        {
            output += m;
        }
        else
        {
            output -= m;
        }
        
        if(tick > n)
            tick = 0;
        else tick += 1;
        
        return output;
    }

    struct Flanger
    {

        DspFloatType SR;        
        DspFloatType LFO;
        DspFloatType depth = 0.43;
	    DspFloatType rate = 2.5;
	    DspFloatType mix = 0.4;
	    DspFloatType delay = 0.0093;
	    DspFloatType feedback = 0.5;
        int   lfoSize=20;
        DspFloatType inputData;
        DspFloatType flangedData;
        FractionalDelay delayL,delayR;
        ToneGenerator   lfoL,lfoR;
        size_t TotalNumInputChannels=2;
        size_t TotalNumOutputChannels=2;

        Flanger(DspFloatType sr)
        {
            SR = sr;
            rate = 20.0/sr;
            delayL.setBufferSize(SR);
            delayR.setBufferSize(SR);            
        }

        //DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        void Process(size_t NumSamples, DspFloatType * input, DspFloatType * output)
        {
            const int totalNumInputChannels  = TotalNumInputChannels;
            const int totalNumOutputChannels = TotalNumOutputChannels;
            
            for (int channel = 0; channel < totalNumInputChannels; ++channel)
            {

                lfoL.initialiseWave(rate, 1, SR, ToneGenerator::SINE);
                lfoR.initialiseWave(rate, 1, SR, ToneGenerator::SINE);

                DspFloatType* channelData = output;
                
                for (int i = 0; i < NumSamples; i++){
                                                    
                    if (channel == 0)
                    {
                        inputData = input[i*TotalNumInputChannels];
                        
                        LFO = (delay * (1 + lfoL.getValue() * depth)) * SR;                
                        LFO = (delay == 0 ? 1 : (LFO > SR ? SR : LFO));
                        
                        flangedData = delayL.getSample(LFO);
                        delayL.addSampleToBuffer(inputData + (feedback * flangedData));
                        
                    }
                    else
                    {
                        inputData = input[i*TotalNumInputChannels+1];
                                        
                        LFO = (delay * (1 + lfoR.getValue() * depth)) * SR;              
                        LFO = (delay == 0 ? 1 : (LFO > SR ? SR : LFO));
                        
                        flangedData = delayR.getSample(LFO);
                        delayR.addSampleToBuffer(inputData + (feedback * flangedData));
                    }

                    channelData[i*TotalNumOutputChannels + channel] = (flangedData * mix) + (inputData * (1 - mix));
                }    
            }
        }
    };
}