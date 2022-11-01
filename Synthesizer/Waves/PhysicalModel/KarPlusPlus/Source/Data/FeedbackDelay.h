#pragma once
#include <cmath> // Used for tanh()

class Delay
{
public:
    
    // ====== CONSTRUCTOR / DESTRUCTOR =======
    Delay() {}
    
    virtual ~Delay()
    {
        delete buffer;      // clean up when the object is destroyed to avoid memory leaks
    }
    
    // ====== SETTER FUNCTIONS =======
    void setSamplerate (float samplerate)
    {
        sr = samplerate; // Assign to private member variable

        smoothDelaytime.reset (sr, 0.02f); // Set samplerate and smoothing of 20ms
        smoothDelaytime.setCurrentAndTargetValue (0.0); // will be overwritten

        smoothFeedback.reset (sr, 0.05f); // Smoothing of 50ms
        smoothFeedback.setCurrentAndTargetValue (0.0);
    }
    
    void setSize(float newSize)
    {
        size = newSize; // Assign to private member variable
        buffer = new float[size]; // Buffer size
        for (int i = 0; i < size; i++) // Iterate through size of buffer
        {
            buffer[i] = 0.0f;
        }
    }
    
    void setDelayTimeInSamples (float delTime)
    {
        smoothDelaytime.setTargetValue (delTime);
        delayTimeInSamples = smoothDelaytime.getNextValue(); // Smoothed value

        readPos = writePos - delayTimeInSamples; // Calculate delay
        while (readPos < 0)
        readPos += size; // Calculate max. size
    }
    
    void setFeedback (float fb) // Value between 0-1 if not running through tanh() or similar
    {
        smoothFeedback.setTargetValue (fb);
        float smoothedFeedback = smoothFeedback.getNextValue(); // Smoothed value

        feedback = smoothedFeedback; // Assign to private member variable

        if (feedback > 10) // Arbitrary protection
            feedback = 10.0f;
        if (feedback < 0)
            feedback = 0.0f;
    }
    
    // ====== UTILITY FUNCTIONS =======
    float readVal()
    {
        float outVal = buffer[readPos]; // Output read position
        readPos++; // Increment read position
        readPos %= size; // Set read position relative to max size
        return outVal;
    }

    void writeVal(float inSamp)
    {
        buffer[writePos] = inSamp; // write into buffer
        writePos++;  // Increment write position
        writePos %= size; // Set write position relative to max size
    }
    
    // ====== CIRCULAR BUFFER - CAN BE REPLACED OR RE-USED =======
    virtual float process (float& inSamp)
    {
        float outVal = readVal();
        writeVal (inSamp + feedback * outVal); // Feedback scales output back into input
        float floor (outVal); // Calculate interpolation
        return floor;
    }
    
protected:
    float* buffer; // Pointer to a buffer - should be uniquePtr
    int size; // Buffer Size
    
    int writePos = 0;  // Write Position
    int readPos = 0.0f;  // Read Position
    
    int delayTimeInSamples;
    
    float feedback = 0.0f; 

    juce::SmoothedValue<float> smoothDelaytime, smoothFeedback;;

    int sr; // Samplerate
    
};

