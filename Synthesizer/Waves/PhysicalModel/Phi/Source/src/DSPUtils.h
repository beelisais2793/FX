/*
  ==============================================================================

    DSPUtils.h
    Created: 25 Apr 2020 1:16:41pm
    Author:  Alexandre Rodrigues

  ==============================================================================
*/

#pragma once

static const float clip(const float input, const float min, const float max) noexcept
{
    return (input > max) ? max : ( (input < min) ? min : input );
}

static const float mixf(float outputWhenInterp0, float outputWhenInterp1, float interp)
{
    // Clip at 0-1
    interp = clip(interp, 0.0f, 1.0f);
    
    return outputWhenInterp1 * interp + outputWhenInterp0 * ( 1.0f - interp );
};

template <typename Type>
class DelayLine
{
public:
    void clear() noexcept
    {
        std::fill (rawData.begin(), rawData.end(), Type (0));
    }

    size_t size() const noexcept
    {
        return rawData.size();
    }

    void resize (size_t newValue)
    {
        rawData.resize (newValue);
        leastRecentIndex = 0;
    }

    Type back() const noexcept
    {
        return rawData[leastRecentIndex];
    }

    Type get (size_t delayInSamples) const noexcept
    {
        jassert (delayInSamples >= 0 && delayInSamples < size());

        return rawData[(leastRecentIndex + 1 + delayInSamples) % size()];
    }
    
    Type getInterpolated (float delayInSamples) const noexcept
    {
        jassert (delayInSamples >= 0.0f && delayInSamples < static_cast<float>(size()));
        
        float position = static_cast<float>(leastRecentIndex + 1) + delayInSamples;
        float lowIndex = floor(position);
        size_t index = static_cast<size_t>(lowIndex);
        
        return mixf(rawData[index % size()], rawData[(index + 1) % size()], position - lowIndex);
    }

    /** Set the specified sample in the delay line */
    void set (size_t delayInSamples, Type newValue) noexcept
    {
        jassert (delayInSamples >= 0 && delayInSamples < size());

        rawData[(leastRecentIndex + 1 + delayInSamples) % size()] = newValue;
    }

    /** Adds a new value to the delay line, overwriting the least recently added sample */
    void push (Type valueToAdd) noexcept
    {
        rawData[leastRecentIndex] = valueToAdd;
        leastRecentIndex = leastRecentIndex == 0 ? size() - 1 : leastRecentIndex - 1;
    }

private:
    std::vector<Type> rawData;
    size_t leastRecentIndex = 0;
};

template <typename Type>
class DCBlock
{
public:
    Type process(Type input)
    {
        previousOutput = input - previousInput + 0.995 * previousOutput;
        previousInput = input;
        return previousOutput;
    }
    void reset()
    {
        previousInput = 0.0;
        previousOutput = 0.0;
    }
private:
    Type previousInput = 0.0;
    Type previousOutput = 0.0;
};

template <typename Type>
class OnePole
{
public:
    Type process(Type input, float cutoff)
    {
        previous = mixf(input, previous, cutoff);
        return dcBlock.process(previous);
    }
    
    Type processInverse(Type input, float cutoff)
    {
        return input - process(input, cutoff);
    }
    
    void reset()
    {
        previous = 0.0;
        dcBlock.reset();
    }
private:
    Type previous = 0.0;
    DCBlock<Type> dcBlock;
};

template <typename Type>
class Accum
{
public:
    Type process(Type input)
    {
        return input + previous;
    }
    
    void reset()
    {
        previous = 0.0;
    }
private:
    Type previous = 0.0;
};
