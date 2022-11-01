#pragma once 

#include <cmath>

namespace SoundWave::Envelopes
{
    template<class T>
    class TADSR 
    {
    public:
        TADSR(void);
        ~TADSR(void);
        T process(void);
        T getOutput(void);
        int getState(void);
        void gate(int on);
        void setAttackRate(T rate);
        void setDecayRate(T rate);
        void setReleaseRate(T rate);
        void setSustainLevel(T level);
        void setTargetRatioA(T targetRatio);
        void setTargetRatioDR(T targetRatio);
        void reset(void);

        enum envState {
            env_idle = 0,
            env_attack,
            env_decay,
            env_sustain,
            env_release
        };

    float Tick(float in) {
        return process() * in;
    }
    float Tick() {
        return process();
    }
    void Process(size_t n, T * input, T * output) {
        for(size_t i = 0; i < n; i++)
        {
            output[i] = process()*input[i];
        }
    }
    void Process(float * samples, size_t n) {
        for(size_t i = 0; i < n ; i++)
            samples[i] = process();
    }

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
    
        T calcCoef(T rate, T targetRatio);
    };

    template<typename T>
     T TADSR<T>::process() {
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
    void TADSR<T>::gate(int gate) {
        if (gate)
            state = env_attack;
        else if (state != env_idle)
            state = env_release;
    }

    template<typename T>
    int TADSR<T>::getState() {
        return state;
    }

    template<typename T>
    void TADSR<T>::reset() {
        state = env_idle;
        output = 0.0;
    }

    template<typename T>
    T TADSR<T>::getOutput() {
        return output;
    }

    template<typename T>
    TADSR<T>::TADSR(void) {
        reset();
        setAttackRate(0);
        setDecayRate(0);
        setReleaseRate(0);
        setSustainLevel(1.0);
        setTargetRatioA(0.3);
        setTargetRatioDR(0.0001);
    }

    template<typename T>
    TADSR<T>::~TADSR(void) {
    }

    template<typename T>
    void TADSR<T>::setAttackRate(T rate) {
        attackRate = rate;
        attackCoef = calcCoef(rate, targetRatioA);
        attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
    }

    template<typename T>
    void TADSR<T>::setDecayRate(T rate) {
        decayRate = rate;
        decayCoef = calcCoef(rate, targetRatioDR);
        decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
    }

    template<typename T>
    void TADSR<T>::setReleaseRate(T rate) {
        releaseRate = rate;
        releaseCoef = calcCoef(rate, targetRatioDR);
        releaseBase = -targetRatioDR * (1.0 - releaseCoef);
    }

    template<typename T>
    T TADSR<T>::calcCoef(T rate, T targetRatio) {
        return (rate <= 0) ? 0.0 : exp(-log((1.0 + targetRatio) / targetRatio) / rate);
    }

    template<typename T>
    void TADSR<T>::setSustainLevel(T level) {
        sustainLevel = level;
        decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
    }

    template<typename T>
    void TADSR<T>::setTargetRatioA(T targetRatio) {
        if (targetRatio < 0.000000001)
            targetRatio = 0.000000001;  // -180 dB
        targetRatioA = targetRatio;
        attackCoef = calcCoef(attackRate, targetRatioA);
        attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
    }

    template<typename T>
    void TADSR<T>::setTargetRatioDR(T targetRatio) {
        if (targetRatio < 0.000000001)
            targetRatio = 0.000000001;  // -180 dB
        targetRatioDR = targetRatio;
        decayCoef = calcCoef(decayRate, targetRatioDR);
        releaseCoef = calcCoef(releaseRate, targetRatioDR);
        decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
        releaseBase = -targetRatioDR * (1.0 - releaseCoef);
    }

    
    #if !defined( M_2PI )
        #define M_2PI 6.28318530717958648
    #endif // !defined( M_2PI )

    template<typename T>
    class TGammaEnvelope
    {
    public:
        T Attack; ///< Attack time, in seconds.
        T Release; ///< Release time, in seconds.
        T AttackDelay; ///< Attack's delay stage percentage [0; 0.25].
        T ReleaseDelay; ///< Release's delay stage percentage [0; 0.25].
        bool IsInverse; ///< "True" if the input signal is inversed (compression
            /// signal).

    protected:
        static const int envcount = 4; ///< The number of envelopes in use.
        static const int envcount4 = envcount * 4; ///< =envcount * 4 (fixed).
        T env[ envcount4 ]; ///< Signal envelope stages 1-4.
        T enva[ 4 ]; ///< Attack stage envelope multipliers 1-4.
        T envb[ 4 ]; ///< Release stage envelope multipliers 1-4.
        T envr[ envcount4 ]; ///< Signal envelope (release) stages 1-4.
        T env5; ///< Signal envelope stage 5.
        T enva5; ///< Attack stage envelope multiplier 5.
        T envb5; ///< Release stage envelope multiplier 5.
        T envr5; ///< Signal envelope (release) stage 5.
        T prevr; ///< Previous output (release).

        /**
         * Function calculates low-pass filter coefficients (multipliers) for the
         * specified SampleRate, Time and o values. This function's implementation
         * is based on a set of tabulated values transformed into formulas. Hence
         * it may not be useful to explore this function, because the original
         * tabulated values were auto-generated via non-linear optimization: while
         * these values are useful (they just work), they are not descriptive of
         * the underlying regularity.
         *
         * @param SampleRate Sample rate.
         * @param Time Envelope's time in seconds.
         * @param o Envelope's delay in percent [0; 0.25].
         * @param[out] envs Resulting envelope multipliers 1-4.
         * @param[out] envs5 Resulting envelope multiplier 5.
         */

        static void calcMults( const T SampleRate, const T Time,
            const T o, T* const envs, T& envs5 )
        {
            const T o2 = o * o;

            if( o <= 0.074 )
            {
                envs[ 3 ] = 0.44548 + 0.00920770 * cos( 90.2666 * o ) -
                    3.18551 * o - 0.132021 * cos( 377.561 * o2 ) -
                    90.2666 * o * o2 * cos( 90.2666 * o );
            }
            else
            if( o <= 0.139 )
            {
                envs[ 3 ] = 0.00814353 + 3.07059 * o + 0.00356226 *
                    cos( 879.555 * o2 );
            }
            else
            if( o <= 0.180 )
            {
                envs[ 3 ] = 0.701590 + o2 * ( 824.473 * o * o2 - 11.8404 );
            }
            else
            {
                envs[ 3 ] = 1.86814 + o * ( 84.0061 * o2 - 10.8637 ) -
                    0.0122863 / o2;
            }

            const T e3 = envs[ 3 ];

            envs[ 0 ] = 0.901351 + o * ( 12.2872 * e3 + o * ( 78.0614 -
                213.130 * o ) - 9.82962 ) + e3 * ( 0.024808 *
                exp( 7.29048 * e3 ) - 5.4571 * e3 );
            const T e0 = envs[ 0 ];

            const T e3exp = exp( 1.31354 * e3 + 0.181498 * o );
            envs[ 1 ] = e3 * ( e0 * ( 2.75054 * o - 1.0 ) - 0.611813 * e3 *
                e3exp ) + 0.821369 * e3exp - 0.845698;
            const T e1 = envs[ 1 ];

            envs[ 2 ] = 0.860352 + e3 * ( 1.17208 - 0.579576 * e0 ) + o * ( e0 *
                ( 1.94324 - 1.95438 * o ) + 1.20652 * e3 ) - 1.08482 * e0 -
                2.14670 * e1;

            if( o >= 0.0750 )
            {
                envs5 = 0.00118;
            }
            else
            {
                envs5 = e0 * ( 2.68318 - 2.08720 * o ) + 0.485294 * log( e3 ) +
                    3.5805e-10 * exp( 27.0504 * e0 ) - 0.851199 - 1.24658 * e3 -
                    0.885938 * log( e0 );
            }

            const T c = M_2PI / SampleRate;
            envs[ 0 ] = calcLP1CoeffLim( c / ( Time * envs[ 0 ]));
            envs[ 1 ] = calcLP1CoeffLim( c / ( Time * envs[ 1 ]));
            envs[ 2 ] = calcLP1CoeffLim( c / ( Time * envs[ 2 ]));
            envs[ 3 ] = calcLP1CoeffLim( c / ( Time * envs[ 3 ]));
            envs5 = calcLP1CoeffLim( c / ( Time * envs5 ));
        }

        /**
         * Function calculates first-order low-pass filter coefficient for the
         * given Theta frequency (0 to pi, inclusive). Returned coefficient in the
         * form ( 1.0 - coeff ) can be used as a coefficient for a high-pass
         * filter. This ( 1.0 - coeff ) can be also used as a gain factor for the
         * high-pass filter so that when high-passed signal is summed with
         * low-passed signal at the same Theta frequency the resuling sum signal
         * is unity.
         *
         * @param theta Low-pass filter's circular frequency, >= 0.
         */

        static T calcLP1Coeff( const T theta )
        {
            const T costheta2 = 2.0 - cos( theta );
            return( 1.0 - ( costheta2 - sqrt( costheta2 * costheta2 - 1.0 )));
        }

        /**
         * Function checks the supplied parameter, limits it to "pi" and calls the
         * calcLP1Coeff() function.
         *
         * @param theta Low-pass filter's circular frequency, >= 0.
         */

        static T calcLP1CoeffLim( const T theta )
        {
            return( calcLP1Coeff( theta < M_PI ? theta : M_PI ));
        }

    public:
        /**
         * Function initializes or updates the internal variables. All public
         * variables have to be defined before calling this function. The clear()
         * function is needed to be called after the first init() function call.
         *
         * @param SampleRate Sample rate.
         */

        void init( const T SampleRate )
        {
            T a;
            T adly;
            T b;
            T bdly;

            if( Attack < Release )
            {
                a = Attack;
                b = Release;
                adly = AttackDelay;
                bdly = ReleaseDelay;
            }
            else
            {
                b = Attack;
                a = Release;
                bdly = AttackDelay;
                adly = ReleaseDelay;
            }

            calcMults( SampleRate, a, adly, enva, enva5 );
            calcMults( SampleRate, b, bdly, envb, envb5 );
        }

        /**
         * Function clears state of *this object.
         *
         * @param initv Initial state value.
         */

        void clear( const T initv )
        {
            int i;

            for( i = 0; i < envcount4; i += 4 )
            {
                env[ i + 0 ] = initv;
                env[ i + 1 ] = initv;
                env[ i + 2 ] = initv;
                env[ i + 3 ] = initv;
                envr[ i + 0 ] = initv;
                envr[ i + 1 ] = initv;
                envr[ i + 2 ] = initv;
                envr[ i + 3 ] = initv;
            }

            env5 = initv;
            envr5 = initv;
            prevr = initv;
        }

        /**
         * Function performs 1 sample processing and produces output sample.
         *
         * @param v Input sample.
         */

        T process( T v )
        {
            const T resa = processSymm( v );
            const T cres = ( IsInverse ? resa <= prevr : resa >= prevr );
            int i;

            if( cres )
            {
                for( i = 0; i < envcount4; i += 4 )
                {
                    envr[ i + 0 ] = resa;
                    envr[ i + 1 ] = resa;
                    envr[ i + 2 ] = resa;
                    envr[ i + 3 ] = resa;
                }

                envr5 = resa;
                prevr = resa;

                return( resa );
            }

            envr[ 0 ] += ( v - envr[ 0 ]) * envb[ 0 ];
            envr[ 1 ] += ( envr5 - envr[ 1 ]) * envb[ 1 ];
            envr[ 2 ] += ( envr[ 4 * 3 + 1 ] - envr[ 2 ]) * envb[ 2 ];
            envr[ 3 ] += ( envr[ 4 * 3 + 0 ] - envr[ 3 ]) * envb[ 3 ];
            envr5 += ( envr[ 4 * 3 + 0 ] - envr5 ) * envb5;

            for( i = 4; i < envcount4; i += 4 )
            {
                envr[ i + 0 ] += ( envr[ i - 4 ] - envr[ i + 0 ]) * envb[ 0 ];
                envr[ i + 1 ] += ( envr[ i - 3 ] - envr[ i + 1 ]) * envb[ 1 ];
                envr[ i + 2 ] += ( envr[ i - 2 ] - envr[ i + 2 ]) * envb[ 2 ];
                envr[ i + 3 ] += ( envr[ i - 1 ] - envr[ i + 3 ]) * envb[ 3 ];
            }

            prevr = envr[ i - 4 ] + envr[ i - 3 ] + envr[ i - 2 ] -
                envr[ i - 1 ] - envr5;

            return( prevr );
        }

        /**
         * Function performs 1 sample processing and produces output sample
         * (symmetric mode, attack and release should be equal).
         *
         * @param v Input sample.
         */

        T processSymm( T v )
        {
    //		_mm_empty(); // May be needed depending on the compiler output.
            env[ 0 ] += ( v - env[ 0 ]) * enva[ 0 ];
            env[ 1 ] += ( env5 - env[ 1 ]) * enva[ 1 ];
            env[ 2 ] += ( env[ 4 * 3 + 1 ] - env[ 2 ]) * enva[ 2 ];
            env[ 3 ] += ( env[ 4 * 3 + 0 ] - env[ 3 ]) * enva[ 3 ];
            env5 += ( env[ 4 * 3 + 0 ] - env5 ) * enva5;
            int i;

            for( i = 4; i < envcount4; i += 4 )
            {
                env[ i + 0 ] += ( env[ i - 4 ] - env[ i + 0 ]) * enva[ 0 ];
                env[ i + 1 ] += ( env[ i - 3 ] - env[ i + 1 ]) * enva[ 1 ];
                env[ i + 2 ] += ( env[ i - 2 ] - env[ i + 2 ]) * enva[ 2 ];
                env[ i + 3 ] += ( env[ i - 1 ] - env[ i + 3 ]) * enva[ 3 ];
            }

            return( env[ i - 4 ] + env[ i - 3 ] + env[ i - 2 ] -
                env[ i - 1 ] - env5 );
        }

        float Tick(float in) {
            return process(in);
        }
        void Process(size_t n, float * input, float * output) {
            for(size_t i = 0; i < n; i++)
            {
                output[i] = process(input[i]);
            }
        }
        void Process(float * samples, size_t n) {
            for(size_t i = 0; i < n ; i++)
                samples[i] = process(samples[i]);
        }

        float TickS(float in) {
            return processSymm(in);
        }
        void ProcessS(size_t n, float * input, float * output) {
            for(size_t i = 0; i < n; i++)
            {
                output[i] = processSymm(input[i]);
            }
        }
        void ProcessS(float * samples, size_t n) {
            for(size_t i = 0; i < n ; i++)
                samples[i] = processSymm(samples[i]);
        }

    };

   
    template<typename T>
    class TParametricEnvelope {
    public:
        enum state_enum {
            idle, attack, decay, sustain, release
        };

        struct parameter_struct {
            T attackTime = 1.0;
            T attackSlope = 0.0;
            T decayTime = 1.0;
            T decaySlope = 0.0;
            T sustainLevel = 0.5;
            T releaseTime = 1.0;
            T releaseSlope = 0.0;
        };

    private:
        
        T currentValue;
        T releaseInitialValue;
        unsigned long currentStep;
        state_enum currentState;
        state_enum previousState;

        // Amplitude scale
        T maxLevel;
        // Time scale. Essentially, how often is step() called?
        unsigned int stepFrequency;
        parameter_struct parameters;

        void gotoState(state_enum newState);

        T calculateAttackValue(unsigned long currentStep, T time, T slope);

        T calculateDecayValue(unsigned long currentStep, T time, T slope, T targetLevel);

        T calculateReleaseValue(unsigned long currentStep, T time, T slope, T originLevel);

    public:
        
        TParametricEnvelope(T maxLevel = 10.0, unsigned int stepFrequency = 10000);

        //
        // Setters for all envelope parameters, with reasonable defaults
        //
        void setAttackTime(T time);

        void setAttackSlope(T slope);

        void setDecayTime(T time);

        void setDecaySlope(T slope);

        void setSustainLevel(T level);

        void setReleaseTime(T time);

        void setReleaseSlope(T slope);

        //
        // Get current time _within the current phase_
        // NB This gets reset at the beginning of the A, D and R phases and isn't updated during S
        T getCurrentTime();

        //
        // Even handlers for gate transitions
        // onGateOn will transition the envelop into STATE_ATTACK
        //
        void onGateOn();

        //
        // onGateOff will transition the envelope into STATE_RELEASE
        //
        void onGateOff();

        //
        // step is called at regular intervals to get the current signal value
        //
        T step();

        //
        // Reset the envelope
        //
        void reset();

        //
        // Get the current state.
        //
        state_enum getState();

        //
        // Return current parameters
        parameter_struct getParameters();

        float Tick(float in) {
            return step()*in;
        }
        float Tick() {
            return step();
        }
        void Process(size_t n, float * input, float * output) {
            for(size_t i = 0; i < n; i++)
            {
                output[i] = input[i]*step();
            }
        }
        void Process(float * samples, size_t n) {
            for(size_t i = 0; i < n ; i++)
                samples[i] = step();
        }

    };

    template<typename T>
    TParametricEnvelope<T>::TParametricEnvelope(T maxLevel, unsigned int stepFrequency) {
        this->maxLevel = maxLevel;
        this->stepFrequency = stepFrequency;
        reset();
    }

    template<typename T>
    void TParametricEnvelope<T>::reset() {
        currentState = idle;
        previousState = idle;
        currentValue = 0.0;
        currentStep = 0;
    }

    template<typename T>
    void TParametricEnvelope<T>::setAttackTime(T time) {
        parameters.attackTime = time;
    }

    template<typename T>
    void TParametricEnvelope<T>::setAttackSlope(T slope) {
        parameters.attackSlope = slope;
    }

    template<typename T>
    void TParametricEnvelope<T>::setDecayTime(T time) {
        parameters.decayTime = time;
    }

    template<typename T>
    void TParametricEnvelope<T>::setDecaySlope(T slope) {
        parameters.decaySlope = slope;
    }

    template<typename T>
    void TParametricEnvelope<T>::setReleaseTime(T time) {
        parameters.releaseTime = time;
    }

    template<typename T>
    void TParametricEnvelope<T>::setReleaseSlope(T slope) {
        parameters.releaseSlope = slope;
    }

    template<typename T>
    void TParametricEnvelope<T>::setSustainLevel(T level) {
        if (level>maxLevel) {
            parameters.sustainLevel = 1.0;
        } else {
            parameters.sustainLevel = level/maxLevel;
        }
    }

    template<typename T>
    typename TParametricEnvelope<T>::parameter_struct TParametricEnvelope<T>::getParameters() {
        return parameters;
    }

    template<typename T>
    typename TParametricEnvelope<T>::state_enum TParametricEnvelope<T>::getState() {
        return currentState;
    }

    template<typename T>
    T TParametricEnvelope<T>::step() {
        //TODO Implement value update
        switch (currentState) {
            case idle:
                break;
            case attack:
                if (previousState == idle) {
                    currentStep = 0;
                    previousState = attack;
                }
                currentValue = calculateAttackValue(currentStep, parameters.attackTime, parameters.attackSlope);
                if (currentValue >= 1.0) {
                    gotoState(decay);
                } else {
                    currentStep++;
                }
                break;
            case decay:
                if (previousState != decay) {
                    currentStep = 0;
                    previousState = decay;
                }
                currentValue = calculateDecayValue(currentStep, parameters.decayTime, parameters.decaySlope,
                                                parameters.sustainLevel);
                if (currentValue <= parameters.sustainLevel) {
                    gotoState(sustain);
                } else {
                    currentStep++;
                }
                break;
            case sustain:
                if (previousState != sustain) {
                    previousState = sustain;
                    currentValue = parameters.sustainLevel;
                }
                break;
            case release:
                if (previousState != release) {
                    currentStep = 0;
                    previousState = release;
                    releaseInitialValue = currentValue;
                }
                currentValue = calculateReleaseValue(currentStep, parameters.releaseTime, parameters.releaseSlope,
                                                    releaseInitialValue);
                if (currentValue < 0.0) {
                    currentValue = 0.0;
                    gotoState(idle);
                } else {
                    currentStep++;
                }
        }
        return currentValue * maxLevel;
    }

    template<typename T>
    T TParametricEnvelope<T>::getCurrentTime() {
        return currentStep / (T)stepFrequency;
    }

    template<typename T>
    void TParametricEnvelope<T>::onGateOn() {
        gotoState(attack);
    }

    template<typename T>
    void TParametricEnvelope<T>::onGateOff() {
        gotoState(release);
    }

    template<typename T>
    void TParametricEnvelope<T>::gotoState(state_enum newState) {
        previousState = currentState;
        currentState = newState;
    }

    template<typename T>
    T TParametricEnvelope<T>::calculateAttackValue(unsigned long currentStep, T time, T slope) {
        return std::pow(getCurrentTime() / time, std::pow(2.0, -slope));
    }

    template<typename T>
    T TParametricEnvelope<T>::calculateDecayValue(unsigned long currentStep, T time, T slope, T targetLevel) {
        return std::pow(getCurrentTime() / time, std::pow(2.0, -slope)) * (targetLevel - 1.0) + 1.0;
    }

    template<typename T>
    T TParametricEnvelope<T>::calculateReleaseValue(unsigned long currentStep, T time, T slope, T originLevel) {
        return originLevel * ( 1- std::pow(getCurrentTime() / time, std::pow(2.0, -slope))) ;
    }
};

        

