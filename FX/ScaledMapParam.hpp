#pragma once

namespace Smoothers
{
    
    template<typename T, typename PQ = ParamQuantity>
    struct ScaledMapParam {
        PQ* paramQuantity = NULL;
        DspFloatType limitMin;
        T limitMinT;
        DspFloatType limitMax;
        T limitMaxT;
        T uninit;
        DspFloatType min = 0.f;
        DspFloatType max = 1.f;

        // in rackdsp.hpp
        dsp::ExponentialSlewLimiter filter;
        bool filterInitialized;
        DspFloatType filterSlew;
        T valueIn;
        DspFloatType value;
        DspFloatType valueOut;

        ScaledMapParam() {
            reset();
        }

        void setLimits(T min, T max, T uninit) {
            limitMin = DspFloatType(min);
            limitMinT = min;
            limitMax = DspFloatType(max);
            limitMaxT = max;
            this->uninit = uninit;
        }
        T getLimitMin() {
            return limitMinT;
        }
        T getLimitMax() {
            return limitMaxT;
        }

        void reset(bool resetSettings = true) {
            paramQuantity = NULL;
            filter.reset();
            filterInitialized = false;
            valueIn = uninit;
            value = -1.f;
            valueOut = std::numeric_limits<DspFloatType>::infinity();

            if (resetSettings) {
                filterSlew = 0.f;
                min = 0.f;
                max = 1.f;
            }
        }

        void resetFilter() {
            filter.reset();
            filterInitialized = false;
        }

        void setParamQuantity(PQ* pq) {
            paramQuantity = pq;
            if (paramQuantity && valueOut == std::numeric_limits<DspFloatType>::infinity()) {
                valueOut = paramQuantity->getScaledValue();
            }
        }

        void setSlew(DspFloatType slew) {
            filterSlew = slew;
            DspFloatType s = (1.f / slew) * 10.f;
            filter.setRiseFall(s, s);
            if (filterSlew == 0.f) filterInitialized = false;
        }
        DspFloatType getSlew() {
            return filterSlew;
        }

        void setMin(DspFloatType v) {
            min = v;
            if (paramQuantity && valueIn != -1) setValue(valueIn);
        }
        DspFloatType getMin() {
            return min;
        }

        void setMax(DspFloatType v) {
            max = v;
            if (paramQuantity && valueIn != -1) setValue(valueIn);
        }
        DspFloatType getMax() {
            return max;
        }

        void setValue(T i) {
            DspFloatType f = rescale(DspFloatType(i), limitMin, limitMax, min, max);
            f = clamp(f, 0.f, 1.f);
            valueIn = i;
            value = f;
        }

        void process(DspFloatType sampleTime = -1.f, bool force = false) {
            if (valueOut == std::numeric_limits<DspFloatType>::infinity()) return;
            // Set filter from param value if filter is uninitialized
            if (!filterInitialized) {
                filter.out = paramQuantity->getScaledValue();
                // If setValue has not been called yet use the parameter's current value
                if (value == -1.f) value = filter.out;
                filterInitialized = true;
            }
            DspFloatType f = filterSlew > 0.f && sampleTime > 0.f ? filter.process(sampleTime, value) : value;
            if (valueOut != f || force) {
                paramQuantity->setScaledValue(f);
                valueOut = f;
            }
        }

        T getValue() {
            DspFloatType f = paramQuantity->getScaledValue();
            if (isNear(valueOut, f)) return valueIn;
            if (valueOut == std::numeric_limits<DspFloatType>::infinity()) value = valueOut = f;
            f = rescale(f, min, max, limitMin, limitMax);
            f = clamp(f, limitMin, limitMax);
            T i = T(f);
            if (valueIn == uninit) valueIn = i;
            return i;
        }

        DspFloatType getLightBrightness() {
            if (!paramQuantity) return 0.f;
            return valueOut;
        }
    }; // struct ScaledMapParam
}