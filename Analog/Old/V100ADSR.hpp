#pragma once


namespace V100
{
    template<typename T>
    class ADSR {
    public:
        
        ADSR(T sample_rate=44100.0f)
        {        
            sr = sample_rate;
            reset();
            setAttackRate(0);
            setDecayRate(0);
            setReleaseRate(0);
            setSustainLevel(1.0);
            setTargetRatioA(0.3);
            setTargetRatioDR(0.0001);        
        }

        ~ADSR(void) {

        }


        ADSR(T a, T d, T s, T r, T sample_rate=44100.0f, T cutoff=10.0f) {     
            sr = sample_rate;
            reset();
            setAttackRate(a * sample_rate);
            setDecayRate(d * sample_rate);
            setSustainLevel(s);
            setReleaseRate(r * sample_rate);                
            setTargetRatioA(0.3);
            setTargetRatioDR(0.0001);
        }
        
        T process(void);
        T getOutput(void);
        int getState(void);
        void gate(int on);
        
        void setAllTimes(T a, T d, T s, T r) {
            reset();
            setAttackTime(a);
            setDecayTime(d);
            setSustainLevel(s);
            setReleaseTime(r);
        }

        void setAttackTime(T rate)  { setAttackRate(rate*sr);}
        void setDecayTime(T rate)   { setDecayRate(rate*sr); }
        void setReleaseTime(T rate) { setReleaseRate(rate*sr);}

        void setAttackRate(T rate);
        void setDecayRate(T rate);
        void setReleaseRate(T rate) ;
        void setSustainLevel(T level);

        void setTargetRatioA(T targetRatio);
        void setTargetRatioDR(T targetRatio);

        void noteOn() { gate(true); }
        void noteOff() { gate(false); }
        void reset(void);

        T Tick(T in) {
            return process() * in;
        }
        T Tick() {
            return process();
        }

        template<typename T>
        void Process(size_t n, T * input, T* output) {
            for(size_t i = 0; i < n; i++)
            {
                output[i] = process()*input[i];
            }
        }
        
        template<typename T>
        void InplaceProcess(size_t n, T * samples) {
            for(size_t i = 0; i < n ; i++)
                samples[i] = process();
        }
        
        enum envState {
            env_idle = 0,
            env_attack,
            env_decay,
            env_sustain,
            env_release
        };

    protected:    
        int state;
        T output;
        T attackRate;
        T decayRate;
        T releaseRate;
        T attackCoef;
        T decayCoef;
        T releaseCoef;
        T sustainLevel;
        T targetRatioA;
        T targetRatioDR;
        T attackBase;
        T decayBase;
        T releaseBase;
        T sr;

        T calcCoef(T rate, T targetRatio);
    };

    template<typename T>
    inline T ADSR<T>::process() {
        switch (state) {
            case env_idle:
                break;
            case env_attack:
                output = attackBase + output * attackCoef;            
                if (output >= 1.0) {
                    output = 1.0;
                    state = env_decay;
                }
                break;
            case env_decay:
                output = decayBase + output * decayCoef;
                if (output <= sustainLevel) {
                    output = sustainLevel;
                    state = env_sustain;
                }
                break;
            case env_sustain:
                break;
            case env_release:
                output = releaseBase + output * releaseCoef;
                if (output <= 0.0) {
                    output = 0.0;
                    state = env_idle;
                }
        }
        return output;    
    }

    template<typename T>
    inline void ADSR<T>::gate(int gate) {
        if (gate)
            state = env_attack;
        else if (state != env_idle)
            state = env_release;
    }

    template<typename T>
    inline int ADSR<T>::getState() {
        return state;
    }

    template<typename T>
    inline void ADSR<T>::reset() {
        state = env_idle;
        output = 0.0;
    }

    template<typename T>
    inline T ADSR<T>::getOutput() {
        return output;
    }

    template<typename T>
    inline void ADSR<T>::setAttackRate(T rate) {
        attackRate = rate;
        attackCoef = calcCoef(rate, targetRatioA);
        attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
    }

    template<typename T>
    inline void ADSR<T>::setDecayRate(T rate) {
        decayRate = rate;
        decayCoef = calcCoef(rate, targetRatioDR);
        decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
    }

    template<typename T>
    inline void ADSR<T>::setReleaseRate(T rate) {
        releaseRate = rate;
        releaseCoef = calcCoef(rate, targetRatioDR);
        releaseBase = -targetRatioDR * (1.0 - releaseCoef);
    }

    template<typename T>
    inline T ADSR<T>::calcCoef(T rate, T targetRatio) {
        return (rate <= 0) ? 0.0 : exp(-log((1.0 + targetRatio) / targetRatio) / rate);
    }

    template<typename T>
    inline void ADSR<T>::setSustainLevel(T level) {
        sustainLevel = level;
        decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
    }

    template<typename T>
    inline void ADSR<T>::setTargetRatioA(T targetRatio) {
        if (targetRatio < 0.000000001)
            targetRatio = 0.000000001;  // -180 dB
        targetRatioA = targetRatio;
        attackCoef = calcCoef(attackRate, targetRatioA);
        attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
    }

    template<typename T>
    inline void ADSR<T>::setTargetRatioDR(T targetRatio) {
        if (targetRatio < 0.000000001)
            targetRatio = 0.000000001;  // -180 dB
        targetRatioDR = targetRatio;
        decayCoef = calcCoef(decayRate, targetRatioDR);
        releaseCoef = calcCoef(releaseRate, targetRatioDR);
        decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
        releaseBase = -targetRatioDR * (1.0 - releaseCoef);
    }
}

using V100ADSR = V100::ADSR<float>;