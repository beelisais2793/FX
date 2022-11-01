#pragma once
#include <cstdint>
#include <memory>
#include <cmath>

namespace Delays
{
    class RingBuffer : public FunctionProcessor
    {
    public:
        RingBuffer(size_t max_length) :
            FunctionProcessor(),
            max_length(max_length),
            current_length(max_length),
            buffer(new DspFloatType[max_length]),
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

        void update(DspFloatType sample)
        {
            buffer[head] = sample;
            head = (head + 1u) % current_length;
            tail = (tail + 1u) % current_length;
        }

        DspFloatType get(DspFloatType offset = 0u)
        {
            DspFloatType integerPart;
            DspFloatType fracPart = modf(offset, &integerPart);

            return (1.f - fracPart) * buffer[(tail + (uint8_t)offset) % current_length] +
                (fracPart)*buffer[(tail + (uint8_t)offset + 1u) % current_length];
        }

        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {
            update(I)
            size_t h = getHead();
            setHead(h + X*getCurrentLength());
            size_t t = buffer.getTail();
            setTail(t + Y*getCurrentLength());
            DspFloatType out = get();
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
        DspFloatType* buffer;
        size_t max_length;
        size_t current_length;
        size_t head;
        size_t tail;
    };
}