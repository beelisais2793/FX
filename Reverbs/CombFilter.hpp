/*
  ==============================================================================
    CombFilter.h
    Created: 10 Jun 2020 5:11:52pm
    Author:  mw
  ==============================================================================
*/

#pragma once
#include <cstdint>
#include <memory>
#include <cmath>

namespace FX::Delays
{
    class RingBuffer : public FunctionProcessor
    {
    public:
        RingBuffer(size_t max_length) :
            FunctionProcessor(),
            max_length(max_length),
            current_length(max_length),
            buffer(new double[max_length]),
            head(max_length - 1u),
            tail(0)
        {
            for (size_t i = 0; i < max_length; ++i) {
                buffer[i] = 0.f;
            }
        }

        bool resize(size_t new_length)
        {
            if (new_length > max_length) {
                return false;
            }

            current_length = new_length;
            return true;
        }

        void update(double sample)
        {
            buffer[head] = sample;
            head = (head + 1u) % current_length;
            tail = (tail + 1u) % current_length;
        }

        double get(double offset = 0u)
        {
            double integerPart;
            double fracPart = modf(offset, &integerPart);

            return (1.f - fracPart) * buffer[(tail + (uint8_t)offset) % current_length] +
                (fracPart)*buffer[(tail + (uint8_t)offset + 1u) % current_length];
        }

        double Tick(double I, double A=1, double X=0, double Y=0)
        {
            update(I)
            size_t h = getHead();
            setHead(h + X*getCurrentLength());
            size_t t = buffer.getTail();
            setTail(t + Y*getCurrentLength());
            double out = get();
            setHead(h);
            setTail(t);
            return A*out;
        }
        size_t getHead() { return head; }
        void   setHead(size_t n) { head = n;  head = head % current_length; }
        size_t getTail() { return tail; }
        void   setTail(size_t n) { tail = n; tail = tail % current_length; }
        size_t getCurrentLength() { return current_length; }

    private:
        double* buffer;
        size_t max_length;
        size_t current_length;
        size_t head;
        size_t tail;
    };

    class CombFilter : public FilterProcessor
    {
    public:
        CombFilter(size_t max_length = 44100) :
            FilterProcessor(),
            buffer(RingBuffer(max_length))
        {
            buffer.resize(1000);
        }

        bool resize(size_t new_length)
        {
            return buffer.resize(new_length);
        }

        double processSample(double sample)
        {
            for (int i = 0; i < 8; ++i) {
                buffer.update(sample);
                sample = 0.5f * sample + 0.5f * buffer.get();
            }
            return sample;
        }
        double Tick(double I, double A=1, double X=0, double Y=0) {
            size_t h = buffer.getHead();
            buffer.setHead(h + X*buffer.getCurrentLength());
            size_t t = buffer.getTail();
            buffer.setTail(t + Y*buffer.getCurrentLength());
            double out = processSample(I);
            buffer.setHead(h);
            buffer.setTail(t);
            return A*out;
        }

    private:
        RingBuffer buffer;
    };
}