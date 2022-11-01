
/**
 * @file AllpassFilter.hpp
 * @author Dale Johnson
 * @date ...uhhh
 */

#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace Filters::Allpass
{
    template<typename T>
    T linterp(T a, T b, T f) {
        return a + f * (b - a);
    }

    template<typename T>
    T clip(T a, T min, T max) {
        return a < min ? min : (a > max ? max : a);
    }

    template<typename T>
    T scale(T a, T inMin, T inMax, T outMin, T outMax) {
        return (a - inMin)/(inMax - inMin) * (outMax - outMin) + outMin;
    }

    template<typename T>
    T semitone(T x) {
        return ((int)(x * 12)) * 0.0833333f;
    }

    uint32_t mwcRand(uint32_t& w, uint32_t& z);

    uint32_t mwcRand(uint32_t& w, uint32_t& z) {
        z = 36969 * (z & 65535) + (z >> 16);
        w = 18000 * (w & 65535) + (w >> 16);
        return (z << 16) + w;
    }

    std::string extractDirectoryFromFilePath(const std::string& filepath) {
        std::string directory = "";

        size_t lastSlashIdx;
        #ifdef _WIN32
            lastSlashIdx = filepath.rfind('\\');
        #elif __APPLE__
            lastSlashIdx = filepath.rfind('/');
        #elif __linux__
            lastSlashIdx = filepath.rfind('/');
        #endif

        if (std::string::npos != lastSlashIdx) {
            directory = filepath.substr(0, lastSlashIdx);
        }
        return directory;
    }



    template<typename T = DspFloatType>
    class InterpDelay2 : public MonoFXProcessor {
    public:
        T input = T(0);
        T output = T(0);

        InterpDelay2(uint64_t maxLength = 512, uint64_t initDelayTime = 0) : MonoFXProcessor() {
            l = maxLength;
            buffer = std::vector<T>(l, T(0));
            setDelayTime(initDelayTime);
        }

        DspFloatType Tick(DspFloatType input, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=1) {
            
            int64_t r = w - t;
            if (r < 0) {
                r += l;
            }            
            r = (int64_t)(fabs(Y)*r);
            int64_t upperR = r - 1;
            if (upperR < 0) {
                upperR += l;
            }
            
            DspFloatType output = linterp(buffer[r], buffer[upperR], f);
            buffer[w] = 0.5*(input+X*output);
            ++w;
            if (w == l) {
                w = 0;
            }

            return A*output;
        }

        void ProcessBlock(size_t blockSize, T* inputBuffer, T* outputBuffer) {
            for (uint64_t i = 0; i < blockSize; ++i) {
                buffer[w] = inputBuffer[i];
                int64_t r = w - t;
                if (r < 0) {
                    r += l;
                }

                ++w;
                if (w == l) {
                    w = 0;
                }

                int64_t upperR = r - 1;
                if (upperR < 0) {
                    upperR += l;
                }

                outputBuffer[i] = linterp(buffer[r], buffer[upperR], f);
            }
        }

        T tap(int64_t i) const {
            int64_t j = w - i;
            if (j < 0) {
                j += l;
            }
            return buffer[j];
        }

        void setDelayTime(T newDelayTime) {
            if (newDelayTime >= l) {
                newDelayTime = l - 1;
            }
            if (newDelayTime < 0) {
                newDelayTime = 0;
            }
            t = static_cast<int64_t>(newDelayTime);
            f = newDelayTime - static_cast<T>(t);
        }

        enum {
            PORT_DELAYTIME,
            PORT_TAP,
        };
        void printPorts() {
            printf("PORTS\n");
            printf("setPort  : PORT_DELAYTIME=0\n");
            printf("getPort2 : PORT_TAP=1")
        }
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_DELAYTIME: setDelayTime(v); break;
                default: printf("No port %d\n",port);
            }
        }
        DspFloatType getPort2(int port, DspFloatType v) {
            if(port == PORT_TAP) return tap(v); 
            printf("No port %d\n",port);
            return 0.0;
        }
        void clear() {
            std::fill(buffer.begin(), buffer.end(), T(0));
            input = T(0);
            output = T(0);
        }

    private:
        std::vector<T> buffer;
        int64_t w = 0;
        int64_t t = 0;
        T f = T(0);
        int64_t l = 512;
    };


    template<class T = DspFloatType>
    class AllpassFilter : public FilterProcessor {
    public:
        AllpassFilter() {
            clear();
            gain = 0;
        }

        AllpassFilter(long maxDelay, long initDelay, T gain) : FilterProcessor {
            clear();
            delay = InterpDelay2<T>(maxDelay, initDelay);
            this->gain = gain;
        }

        enum {
            PORT_GAIN
        };
        void printPorts()
        {
            printf("setPort\n");
            printf("PORT_GAIN=0\n");
        }
        void setPort(int port, DspFloatType v) {
            if(port == PORT_GAIN) gain = v;
            else printf("No port %d\n",port);
        }
        DspFloatType Tick(DspFloatType I=1, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            _inSum = input + delay.output * gain;
            output = delay.output + _inSum * gain * -1;
            delay.input = _inSum;
            delay.process();
            return output;
        }

        void ProcessBlock(size_t blockSize,T* inputBuffer, T* outputBuffer) {
            for (uint64_t i = 0; i < blockSize; ++i) {
                _inSum = inputBuffer[i] + delay.output * gain;
                output = delay.output + _inSum * gain * -1;
                delay.input = _inSum;
                delay.process();
                outputBuffer[i] = output;
            }
        }

        void clear() {
            input = 0;
            output = 0;
            _inSum = 0;
            _outSum = 0;
            delay.clear();
        }

        T input;
        T gain;
        T output;
        InterpDelay2<T> delay;
    private:
        T _inSum;
        T _outSum;
    };

    template<class T = DspFloatType>
    class NestedAllPassType1 : public FilterProcessor{
    public:
        NestedAllPassType1() : FilterProcessor() {
            gain1 = 0;
            gain2 = 0;
            decay1 = 0;
            decay2 = 0;
            clear();
        }

        NestedAllPassType1(long maxDelay, long delayTime1, long delayTime2) {
            delay1 = InterpDelay2<T>(maxDelay, delayTime1);
            delay2 = InterpDelay2<T>(maxDelay, delayTime2);
            gain1 = 0;
            gain2 = 0;
            decay1 = 0;
            decay2 = 0;
            clear();
        }

        DspFloatType Tick(DspFloatType I=0, DspFloatType A=0, DspFloatType X=0, DspFloatType Y=0) {
            _inSum1 = input + delay1.output * gain1;
            _inSum2 = _inSum1 + delay2.output * gain2;
            delay2.input = _inSum2;
            delay1.input = delay2.output * decay2 + _inSum2 * -gain2;
            output = delay1.output * decay1 + _inSum1 * -gain1;
            delay1.process();
            delay2.process();
            return output;
        }

        void clear() {
            input = 0;
            output = 0;
            _inSum1 = 0;
            _inSum2 = 0;
            delay1.clear();
            delay2.clear();
        }

        T input;
        T gain1, gain2;;
        T output;
        T decay1, decay2;
        InterpDelay2<T> delay1, delay2;
    private:
        T _inSum1, _inSum2;
    };
}

