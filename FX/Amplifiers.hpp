///////////////////////////////////////////////////////////////////////////
// Amplifiers
// Clippers 
// Distortion
// Waveshapers
// Wave Digital Filters
///////////////////////////////////////////////////////////////////////////
#pragma once
#include <functional>
#include <random>
#include "ClipFunctions.hpp"
#include "SoundObject.hpp"
// this was a bad idea
DspFloatType pre_gain  = 1.0f;
DspFloatType post_gain = 1.0;


namespace FX::Distortion
{
    struct AmplifierFunction1 : public AmplifierProcessor
    {
        std::function<DspFloatType (DspFloatType I)> func;
        AmplifierFunction1() : AmplifierProcessor() {

        }
        AmplifierFunction1(std::function<DspFloatType (DspFloatType I)> f) : AmplifierProcessor(),func(f)
        {

        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            return A*func(I);
        }
    };
    struct AmplifierFunction2 : public AmplifierProcessor
    {
        std::function<DspFloatType (DspFloatType I, DspFloatType X)> func;
        AmplifierFunction2() : AmplifierProcessor() {
            
        }
        AmplifierFunction2(std::function<DspFloatType (DspFloatType I, DspFloatType X)> f) : AmplifierProcessor(),func(f)
        {

        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            return A*func(I,X);
        }
    };
    struct AmplifierFunction3 : public AmplifierProcessor
    {
        std::function<DspFloatType (DspFloatType I, DspFloatType X, DspFloatType Y)> func;
        AmplifierFunction3() : AmplifierProcessor() {
            
        }
        AmplifierFunction3(std::function<DspFloatType (DspFloatType I, DspFloatType X, DspFloatType Y)> f) : AmplifierProcessor(),func(f)
        {

        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            return A*func(I,X,Y);
        }
    };
    struct AmplifierFunction4 : public AmplifierProcessor
    {
        std::function<DspFloatType (DspFloatType I, DspFloatType A, DspFloatType X, DspFloatType Y)> func;
        AmplifierFunction4() : AmplifierProcessor() {
            
        }
        AmplifierFunction4(std::function<DspFloatType (DspFloatType I, DspFloatType A, DspFloatType X, DspFloatType Y)> f) : AmplifierProcessor(),func(f)
        {

        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            return func(I,A,X,Y);
        }
    };

    DspFloatType udo1(DspFloatType x, DspFloatType g = 1.0)
    {
        DspFloatType ax = fabs(x);
        if(ax == 0) return 0;
        return (x/ax)*(1-exp(g*(x*x)/ax));
    }

    
    DspFloatType Fold(DspFloatType x)
    {
        if( x > 1) return Fold(1-(x-1));
        if( x < -1) return Fold(-1-(x+1));
        return x;
    }
    DspFloatType Wrap(DspFloatType x) {
        if( x > 1) return fmod(x,1)-1;
        if( x < 1) return 1-fmod(-x,1);
        return x;
    }
    DspFloatType SinFold(DspFloatType x) {
        return sin(2*M_PI*x);
    }
    
    DspFloatType cheby(int n, DspFloatType x)
    {
        if(n == 0) return 1;
        if(n == 1) return x;
        return 2*x*cheby(n-1,x) - cheby(n-2,x);
    }

    inline DspFloatType cheby_polynomial(int n, DspFloatType x)
    {
        if(n == 0) return 1.0;
        if(n == 1) return x;
        return 2*x*cheby_polynomial(n-1,x) - cheby_polynomial(n-2,x);
    }
    
    inline DspFloatType amp_clamp(DspFloatType x, DspFloatType a, DspFloatType b) {
        return x < a? a: x > b? b : x;
    }
    inline DspFloatType preamp(DspFloatType x) {
        return pre_gain * x;
    }
    inline DspFloatType postamp(DspFloatType x) {
        return post_gain * x;
    }
    inline DspFloatType tanh_normal(DspFloatType x, DspFloatType K=10,DspFloatType r = 1.0f) {
        return post_gain*std::tanh(pre_gain*K*x) / std::tanh(r);
    }
    inline DspFloatType positive_signal(DspFloatType x) {
        return (x+1)/2;
    }
    inline DspFloatType negative_signal(DspFloatType x) {
        return (x-1)/2;
    }
    

    inline DspFloatType sigmoid_function(DspFloatType x, DspFloatType K=10) {
        return 2*(1.0f / (1.0f + std::exp(-K*x))-0.5);
    }
    inline DspFloatType sigmoid_minus(DspFloatType x, DspFloatType K=10) {
        return -sigmoid_function(x,K);
    }
    inline DspFloatType bpsigmoid(DspFloatType x, DspFloatType g = 10) {
        return sigmoid_function(-x,g);
    }
    inline DspFloatType full_rectify(DspFloatType x) {
        return amp_clamp(x*std::abs(x),0,1);
    }
    inline DspFloatType half_rectify(DspFloatType x) {
        return amp_clamp(x,0,1);
    }
    inline DspFloatType modulated_signals(DspFloatType a, DspFloatType b) {
        return a*b;
    }
    

    inline DspFloatType circular_modulated_signals(DspFloatType a, DspFloatType b) {
        return std::fmod(a,b);
    }
    inline DspFloatType positive_modulated_signals(DspFloatType a, DspFloatType b) {
        return positive_signal(a)*positive_signal(b);
    }
    inline DspFloatType negative_modulated_signals(DspFloatType a, DspFloatType b) {
        return negative_signal(a)*negative_signal(b);
    }

    inline DspFloatType sigmoidDistortionFunction(DspFloatType x, DspFloatType gain,
                                            DspFloatType max, DspFloatType dc) {                                                        
        return max * gain * x / sqrt(1 + (gain * std::pow(gain * x, 2))) + dc;
    }

    
    inline DspFloatType asymmetricSigmoidDistortionFunction(DspFloatType x) 
    {
        // Cutoff for chopping top
        static DspFloatType cutoff = 0.05;
        static DspFloatType slope = 0.1;
        static DspFloatType gain = 20;
        static DspFloatType max = 0.3;
        static DspFloatType dc = 0;
        // Calculate constant to add to linear region to make it join up with the
        // sigmoid function
        static DspFloatType b = sigmoidDistortionFunction(x, gain, max, dc) - slope * cutoff;
        if (x > cutoff) {
            return slope * x + b;
        } else {
            return sigmoidDistortionFunction(x, gain, max, dc);
        }
    }

    

    inline DspFloatType assymetric_sigmoid(DspFloatType I, DspFloatType A = 1, DspFloatType X = -1, DspFloatType Y = 1) {        
        DspFloatType x = asymmetricSigmoidDistortionFunction(A*pre_gain*I);
        return amp_clamp(post_gain*x,X,Y);
    }
    
    inline DspFloatType asymmetricSigmoidDistortionFunction2(DspFloatType x) {
        // Cutoff for chopping top
        static DspFloatType cutoff = 0.05;
        static DspFloatType gain = 20;
        static DspFloatType max = 0.3;
        static DspFloatType dc = 0;
        
        if (x > cutoff) {
            return sigmoidDistortionFunction(sigmoidDistortionFunction(x, gain, max, dc), gain * 2, max, dc);
        } else {
            return sigmoidDistortionFunction(x, gain, max, dc);
        }
    }
    

    inline DspFloatType assymetric_sigmoid2(DspFloatType I, DspFloatType A = 1, DspFloatType X = -1, DspFloatType Y = 1) {
        DspFloatType x = asymmetricSigmoidDistortionFunction2(A*pre_gain*I);
        return amp_clamp(post_gain*x,X,Y);
    }

    inline DspFloatType distortionFunction(DspFloatType x) {
        if (x < -0.08905) {
            // Assume x >= -1
            // Therefore, this first interval is actually -1 <= x < -0.08905
            return -(3 / 4) * (1 - std::pow((1 - (-x - 0.032847)), 12) +
                            (1 / 3) * (-x - 0.032847)) +
                0.01;
        } else if (x < 0.320018) {
            return -6.153 * std::pow(x, 2) + 3.9375 * x;
        } else {
            // Assume x <= 1
            // Therefore, this last interval is actually 0.320018 <= x <= 1
            return 0.630035;
        }
    }
    
    inline DspFloatType distortion_function(DspFloatType I, DspFloatType A=1,DspFloatType X = -1, DspFloatType Y=1) 
    {
        DspFloatType x = distortionFunction(A*pre_gain*I);
        return amp_clamp(post_gain*x,X,Y);
    }


    DspFloatType cubic_distortion(DspFloatType in, DspFloatType A = 1, DspFloatType X = -1, DspFloatType Y = 1)
    {
        A *= pre_gain;
        DspFloatType r = A*(in - (1.0f/3.0f)*std::pow(in,3.0f));    
        return amp_clamp(post_gain*r,X,Y);
    }
    DspFloatType asin_distortion(DspFloatType in, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
    {
        A *= pre_gain;
        DspFloatType r = (2.f / M_PI) * std::asin(in * A);
        if(r < X) r = X;
        if(r > Y) r = Y;
        return amp_clamp(post_gain*r,X,Y);
    }
    DspFloatType acos_distortion(DspFloatType in, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
    {
        A *= pre_gain;
        DspFloatType r = (2.f / M_PI) * std::acos(in * A);
        if(r < X) r = X;
        if(r > Y) r = Y;
        return amp_clamp(post_gain*r,X,Y);
    }

    
    DspFloatType atan_distortion(DspFloatType in, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
    {
        A *= pre_gain;
        DspFloatType r = (2.f / M_PI) * std::atan(in * A);
        if(r < X) r = X;
        if(r > Y) r = Y;
        return amp_clamp(post_gain*r,X,Y);
    }
    DspFloatType asinh_distortion(DspFloatType in, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
    {
        A *= pre_gain;
        DspFloatType r = (2.f / M_PI) * std::asinh(in * A);    
        return amp_clamp(A*r,X,Y);    
    }
    DspFloatType acosh_distortion(DspFloatType in, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
    {
        A *= pre_gain;
        DspFloatType r = (2.f / M_PI) * std::acosh(in * A);
        if(r < X) r = X;
        if(r > Y) r = Y;
        return amp_clamp(post_gain*r,X,Y);
    }
    DspFloatType atanh_distortion(DspFloatType in, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
    {
        A *= pre_gain;
        DspFloatType r = (2.f / M_PI) * std::atanh(in * A);
        return amp_clamp(post_gain*r,X,Y);
    }
    DspFloatType exp_distortion(DspFloatType x, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
    {
        A *= pre_gain;
        DspFloatType sign = x < 0? -1.0f:1.0f;
        DspFloatType r = sign * (1.f - std::exp(-std::fabs(x * A)));
        return amp_clamp(post_gain*r,X,Y);
    }
    DspFloatType dc_distortion(DspFloatType x, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
    {
        A *= pre_gain;;
        DspFloatType dc = (DspFloatType)rand() / (DspFloatType)(RAND_MAX);
        DspFloatType r = 0;
        if(x < 0) r = cubic_distortion(x - X*dc,A,X,Y);
        else r = cubic_distortion(x+Y*dc,A,X,Y);
        return amp_clamp(post_gain*r,X,Y);
    }
    DspFloatType bipolar_distortion(DspFloatType x, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
    {
        A *= pre_gain;;
        DspFloatType r = 0;
        if(x > 0) r = atan_distortion(x,A,X,Y);
        else r = cubic_distortion(x,A,X,Y);
        return amp_clamp(post_gain*r,X,Y);
    }
    
    
    DspFloatType quadratic_distortion(DspFloatType x, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
    {    
        A *= pre_gain;
        DspFloatType r = x;
        if(x >= 0 && x < M_PI/2) r = atan_distortion(x,A,X,Y);
        else if(x >= (M_PI/2) && x < M_PI) r = atan_distortion(x,A,X,Y);
        else if(x >= M_PI && x < (3*M_PI/4)) r = exp_distortion(x,A,X,Y);
        else r= exp_distortion(x,A,X,Y);
        return amp_clamp(post_gain*r,X,Y);
    }
    DspFloatType quadratic2_distortion(DspFloatType x, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
    {    
        A *= pre_gain;
        DspFloatType r = x;
        if(x >= 0 && x < M_PI/2) r= atan_distortion(x,A,X,Y);
        else if(x >= (M_PI/2) && x < M_PI) r= cubic_distortion(x,A,X,Y);
        else if(x >= M_PI && x < (3*M_PI/4)) r= atan_distortion(x,A,X,Y);
        else r= cubic_distortion(x,A,X,Y);
        return amp_clamp(post_gain*r,X,Y);
    }
    DspFloatType quadratic3_distortion(DspFloatType x, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
    {    
        A *= pre_gain;
        DspFloatType r = x;
        if(x >= 0 && x < M_PI/2) r=cubic_distortion(x,A,X,Y);
        else if(x >= (M_PI/2) && x < M_PI) r=cubic_distortion(x,A,X,Y);
        else if(x >= M_PI && x < (3*M_PI/4)) r=atan_distortion(x,A,X,Y);
        else r= atan_distortion(x,A,X,Y);
        return amp_clamp(post_gain*r,X,Y);
    }
    DspFloatType parametric_clip(DspFloatType input, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
    {
        A *= pre_gain;
        DspFloatType softClip = (2.f / M_PI) * std::atan(input * A);
        DspFloatType blend = input * (X*0.5 + softClip * Y*0.5);
        return amp_clamp(post_gain*blend,X,Y);
    }

    
    DspFloatType arcTanDistortion (DspFloatType input, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y = 1)
    {
        A *= pre_gain;
        DspFloatType gain = A + 1.0f;    
        DspFloatType out = 2.0f / M_PI * std::atan(gain * input);    
        out = out / std::log(gain);
        return amp_clamp(post_gain*out,X,Y);
    }

    DspFloatType softClipper (DspFloatType input, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
    {
        A *= pre_gain;;
        DspFloatType newInput = input * A;
        DspFloatType out = 0.0;
        
        if (newInput >= 1.0f)
            out = 1.0f;
        else if ((newInput > -1) && (newInput < 1))
            out = (3.0f / 2.0f) * (newInput - (std::pow(newInput, 3.0f) / 3.0f));
        else if (newInput <= -1)
            out = -1.0f;
        
        return amp_clamp(post_gain*out,X,Y);    
    }
    
    DspFloatType errorf(DspFloatType x, DspFloatType K = 10, DspFloatType X =-1, DspFloatType Y=1) {
        DspFloatType r = std::erf(pre_gain*K*x);
        r *= post_gain;
        return amp_clamp(r,X,Y);
    }

    
    DspFloatType sigmoided (DspFloatType input, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
    {
        A *= pre_gain;;
        DspFloatType gain = gain + 1.0f;    
        DspFloatType out = (2.0f * (1.0f / (1.0f + std::exp(-gain * input)))) - 1;    
        out = (out) / (std::log(gain));
        return amp_clamp(post_gain*out,X,Y);
    }


    DspFloatType hardclip(DspFloatType input, DspFloatType A=1, DspFloatType X=-1, DspFloatType Y=1) {

        input *= A*pre_gain;
        return amp_clamp(post_gain*input,X,Y);
    }

    DspFloatType hyperbolicTangent (DspFloatType input, DspFloatType gain=1, DspFloatType X = -1, DspFloatType Y=1)
    {
        gain = (gain + 1.0f);
        DspFloatType out = (std::tanh(gain * pre_gain * input)) / (std::tanh(gain));
        return amp_clamp(post_gain*out,X,Y);
    }

    DspFloatType diodeClipping (DspFloatType input, DspFloatType gain=1, DspFloatType X=-1, DspFloatType Y=1)
    {
    //    gain = gain + 1.0f;
        
        DspFloatType diodeClippingAlgorithm = std::exp((0.1f * pre_gain * input) / (0.0253f * 1.68f)) - 1.0f;
        DspFloatType out = 2 / M_PI * std::atan(diodeClippingAlgorithm * (gain * 16));
        return amp_clamp(post_gain*out,X,Y);
    }

    

    DspFloatType fuzzExponential (DspFloatType input, DspFloatType gain=1, DspFloatType X =1, DspFloatType Y=1)
    {
        gain *= pre_gain;
        DspFloatType newInput = input * gain;
        DspFloatType out;
        
        //Soft clipping
        if (newInput < 0.0f)
            out = -1.0f *  (1.0f - std::exp(-abs(newInput)));
        else
            out = 1.0f * (1.0f - std::exp(-abs(newInput)));
    
        //Half Wave Rectifier
        out = 0.5f * (out + abs(out));
        return amp_clamp(post_gain*out,X,Y);

    }
    
    DspFloatType pieceWiseOverdrive (DspFloatType input, DspFloatType gain, DspFloatType X =-1, DspFloatType Y=1)
    {
        gain = (gain + 1.0f);
        DspFloatType newInput = pre_gain*input * (gain) ;
        DspFloatType out = 0.0f;
        
        if (abs(newInput) <= 1.0f / 3.0f)
            out = 2.0f * newInput;
        else if (abs(newInput) > 2.0f / 3.0f)
        {
            if (newInput > 0.0f)
                out = newInput;
            if (newInput < 0.0f)
                out = -newInput;
        } else
        {
            if (newInput > 0.0f)
                out = (3.0f - std::pow((2.0f - newInput * 3.0f), 2.0f)) / 3.0f;
            if (newInput < 0.0f)
                out = -(3.0f - std::pow((2.0f - newInput * 3.0f), 2.0f)) / 3.0f;
        }
        
        out = (out / std::log(gain + 1.0f));
        return amp_clamp(post_gain*out,X,Y);    
    }

    DspFloatType tube (DspFloatType input, DspFloatType gain, DspFloatType X =-1, DspFloatType Y=1)
    {
        gain = (gain + 1.0f);
        DspFloatType Q = -1.5f; //more negative = more linear
        DspFloatType distortion = 5; //higher number = higher distortion
        DspFloatType out;
        
        DspFloatType newInput = pre_gain * input * (gain / 10);
        
        if (Q == 0)
        {
            out = newInput / (1 - std::exp(-distortion * newInput));
            if (newInput == Q)
            {
                out = 1 / distortion;
            }
        } else
        {
            out = ((newInput - Q) / (1 - std::exp(-distortion * (newInput - Q)))) + (Q / (1 - std::exp(distortion * Q)));
            if (newInput == Q)
            {
                out = (1 / distortion) + (Q / (1 - std::exp(distortion * Q)));
            }
        }
        
        return amp_clamp(post_gain*out,X,Y);
    }

    
    

    DspFloatType arraya (DspFloatType input, DspFloatType gain, DspFloatType X =-1, DspFloatType Y=1)
    {
        gain = (gain + 1.0f);
        auto newInput = pre_gain * input;
        
        //Arraya

        auto out = ((3.0f * newInput) / 2.0f) * (1.0f - (std::pow(newInput, 2.0f) / 3.0f));
        
    //    Fuzz Exponential
        if (out < 0.0f)
            out = 1.0f * ((1.0f - std::exp(abs(out)) / (std::exp(1.0f) - 1.0f)));
        else
            out = -1.0f * ((1.0f - std::exp(abs(out)) / (std::exp(1.0f) - 1.0f)));
        
        //Exponential 2
    //    out = (std::exp(1.0f) - std::exp(1.0f - out)) / (std::exp(1.0f) - 1.0f);
        
    //    out = 0.5f * (out + abs(out));
    //    out = abs(out);
        
        if (gain >= 10.0f)
            out = out * (gain / 100.0f);
        else
            out = out * (0.1f);
        
        //Arraya
        out = ((3.0f * out) / 2.0f) * (1.0f - (std::pow(out, 2.0f) / 3.0f));
        return amp_clamp(post_gain*out,X,Y);
    }

    DspFloatType gallo (DspFloatType input, DspFloatType gain, DspFloatType X =-1, DspFloatType Y=1)
    {
        gain = (gain + 1.0f);
        DspFloatType a = -0.01f;
        DspFloatType b = 0.7f;
        DspFloatType k1 = std::pow(a, 2.0f);
        DspFloatType k2 = 1 + (2 * a);
        DspFloatType k3 = std::pow(b, 2.0f);
        DspFloatType k4 = 1 - (2 * b);
        DspFloatType out_1 = 0.0f;
        
        auto newInput = pre_gain * input * gain;
        
        if (newInput < a)
            out_1 = (k1 + newInput) / (k2 - newInput);
        if (newInput >= a && newInput <= b)
            out_1 = newInput;
        if (newInput > b)
            out_1 = (newInput - k3) / (newInput + k4);
        
        return amp_clamp(post_gain*out_1,X,Y);
    }
    
    DspFloatType doubleSoftClipper (DspFloatType input, DspFloatType gain, DspFloatType X =-1, DspFloatType Y=1)
    {
        gain = (gain + 1.0f);
        auto slope = 2.0f;
        auto upperLim = 0.8f;
        auto lowerLim = -1.0f;
        auto upperSkew = 1.0f;
        auto lowerSkew = 1.0f;
        auto xOffFactor = 0.0f;
        auto out = 0.0f;
        
        auto xOff = (1.0f / slope) * std::pow(slope, xOffFactor);
        
        input *= (gain / 10.0f);
        
        if (input > 0.0f)
        {
            input = (input - xOff) * upperSkew;
            
            if (input >= 1.0f / slope)
                out = upperLim;
            else if (input <= -1.0f / slope)
                out = 0.0f;
            else
                out = (3.0f / 2.0f) * upperLim * (slope * input - std::pow(slope * input, 3.0f) / 3.0f) / 2.0f + (upperLim / 2.0f);
        } else
        {
            input = (input + xOff) * lowerSkew;
            
            if (input >= 1.0f / slope)
                out = 0.0f;
            else if (input <= -1.0f / slope)
                out = lowerLim;
            else
                out = (3.0f / 2.0f) * -lowerLim * (slope * input - std::pow(slope * input, 3.0f) / 3.0f) / 2.0f + (lowerLim / 2.0f);
        }
        if(out < X) out = X;
        if(out > Y) out = Y;
        return out;
    }

    DspFloatType crush (DspFloatType input, DspFloatType gain, DspFloatType X =-1, DspFloatType Y=1)
    {
        gain = pre_gain*(gain + 1.0f);
        auto out = 0.0f;
        
        gain /= 100.0f;
        
        DspFloatType dry = input;
        DspFloatType wet = round(input * std::pow(2, gain));
        out = (wet + dry)  * asin(gain) + dry;
        return amp_clamp(post_gain*out,X,Y);
    }

    
    DspFloatType tuboid (DspFloatType input, DspFloatType gain, DspFloatType X =-1, DspFloatType Y=1)
    {
        gain = pre_gain*(gain + 1.0f);
        auto ktp = 1.0f;
        auto ktn = 3.0f;
        auto sfn = 0.0f;
        
        auto threshPos = 0.3f;
        auto threshNeg = -0.7f;
        
        auto out = 0.0f;
        
        gain /= 10.0f;
        
        auto so = input * gain;
        
        if (so >= threshPos)
            sfn = ktp * std::pow(so - threshPos, 3.0f);
        else if (so <= threshNeg)
            sfn = -ktn * abs(std::pow(so - threshNeg, 3.0f));
        else
            sfn = 0.0f;
        
        so = (input - sfn) * gain;
        out = so;    
        return amp_clamp(post_gain*out,X,Y);
    }

    DspFloatType pakarinen_Yeh (DspFloatType input, DspFloatType gain, DspFloatType X =-1, DspFloatType Y=1)
    {
        gain = pre_gain*(gain + 1.0f);
        auto out = 0.0f;
        
        gain /= 100.0f;
        
        auto x = input * gain;
        
        if ((x >= 0.320018f) && (x <= 1.0f))
            out = 0.630035f;
        else if ((x >= -0.08905f) && (x < 0.320018))
            out = (-6.153f * std::pow(x, 2.0f)) + (3.9375f * x);
        else if ((x >= -1.0f) && (x < -0.08905f))
            out = (-0.75f * (1.0f - std::pow(1.0f - (abs(x) - 0.029f), 12.0f) + (0.333f * (abs(x) - 0.029f)))) + 0.01f;
        else
            out = -0.9818f;
        
        out *= 1.5f;
        return amp_clamp(post_gain*out,X,Y);
    }

    
    template<typename T>
    T logiclip (T x) noexcept
    {
        return 2.0f / (1.0f + std::exp (-2.0f * x)) - 1.0f;
    }
    template<typename T>
    T hardclip (T x) noexcept
    {
        return sgn (x) * std::fminf (std::fabs(x), 1.0f);
    }
    template<typename T>
    T tanclip (T x) noexcept
    {
        DspFloatType soft = 0.0f;
        return std::tanh ((1.0f - 0.5f * soft) * x);
    }
    template<typename T>
    T quintic (T x) noexcept
    {
        if (std::fabs (x) < 1.25f)
        {
            return x - (256.0f / 3125.0f) * std::pow (x, 5.0f);
        } else
        {
            return sgn (x) * 1.0f;
        }
    }
    template<typename T>
    T cubicBasic (T x) noexcept
    {
        if (std::fabs (x) < 1.5f)
        {
            return x - (4.0f / 27.0f) * std::pow (x, 3.0f);
        } else
        {
            return sgn (x) * 1.0f;
        }
    }
    
    template<typename T>
    T algClip (T x) noexcept
    {
        DspFloatType soft = 0.0f;
        return x / std::sqrt ((1.0f + 2.0f * soft + std::pow (x, 2.0f)));
    }
    template<typename T>
    T arcClip (T x) noexcept
    {
        DspFloatType soft = 0.0f;
        return (2.0f / M_PI) * std::atan ((1.6f - soft * 0.6f) * x);
    }
    template<typename T>
    T sinclip (T x) noexcept
    {
        if (std::fabs (x) < M_PI)
        {
            return std::sin (x);
        }
        else
        {
            return sgn (x) * 1.0f;
        }
    }
    
    inline DspFloatType FuzzCtrTable(const DspFloatType x)
    {
        static auto gen = std::minstd_rand(2112);
        static const DspFloatType b = 20;
        static auto dist = std::uniform_real_distribution<DspFloatType>(-1.0, 1.0);
        auto g = exp(-x * x * b);
        auto xadj = x + g * dist(gen);
        return xadj;
    }
    template <int scale> DspFloatType FuzzTable(const DspFloatType x)
    {
        static auto gen = std::minstd_rand(2112);
        static const DspFloatType range = 0.1 * scale;
        static auto dist = std::uniform_real_distribution<DspFloatType>(-range, range);
        auto xadj = x * (1 - range) + dist(gen);
        return xadj;
    }
    static DspFloatType absf(DspFloatType x) {
        return (x >= 0.0 ? x : -x);
    }
    // cube function
    static DspFloatType cubef(DspFloatType x) {
        return (x * x * x);
    }
    // use this to process audio via the rectification algorithm
    static DspFloatType Rectify(DspFloatType sample, DspFloatType RectifierThreshold=0.9) {
        return ((1 - RectifierThreshold) * sample) + (absf(sample) * RectifierThreshold);
    };
    // hard clip of input signal
    static DspFloatType HardClip(DspFloatType sample, DspFloatType thresh) {
        return 0.5 * (absf(sample + thresh) - absf(sample - thresh));
    };
    // cubic soft clip function
    static DspFloatType SoftCubicClip(DspFloatType sample, DspFloatType thresh) {
        DspFloatType threshInv = 1 / thresh;
        return threshInv * ((thresh * 1.5 * HardClip(sample, thresh)) -
            (0.5 * cubef(HardClip(sample, thresh)) * threshInv));
    };
    // use this to process audio via the SoftCubicClip algorithm
    static DspFloatType SoftCubic(DspFloatType sample, DspFloatType CubicSoftClipThreshold=0.9, DspFloatType CubicHarmonicBalance=0.5) {
        DspFloatType invsqrt2 = 1.0/sqrt(2);
        return (invsqrt2 / 3) * (SoftCubicClip(sample, CubicSoftClipThreshold) +
            (CubicHarmonicBalance * SoftCubicClip(absf(sample), CubicSoftClipThreshold)));
    };
    // soft clip function with adjustable knee
    static DspFloatType SKClip(DspFloatType sample, DspFloatType knee) {
        return sample / (knee * absf(sample) + 1.0);
    };

    
    // use this to process audio via the SKClip algorithm
    static DspFloatType SoftKnee(DspFloatType sample, DspFloatType SoftClipKnee=0.9) {
        return 0.5 * (SKClip(sample, SoftClipKnee) + ((SoftClipKnee / 2.0) * SKClip(absf(sample), SoftClipKnee)));
    };
    // use this to process audio via the leaky integrator algorithm
    static DspFloatType LeakyInt(DspFloatType sample, DspFloatType previous_sample, DspFloatType TcRise = 0.5, DspFloatType TcFall=0.5) {
        DspFloatType invsqrt2 = 1.0/sqrt(2);
        if (sample > previous_sample) 
        {
        return invsqrt2 * (((1.0 - TcRise) * sample) + (TcRise * previous_sample));
        }
        else {
            return invsqrt2 * (((1.0 - TcFall) * sample) + (TcFall * previous_sample));
        } 
    }

    DspFloatType linearScale( DspFloatType in, DspFloatType min, DspFloatType max )
    {
        DspFloatType ret;
        if ( min == 0.0f && max == 0.0f )
        {
            ret = 0.0f;
        }
        else if ( min > max )
        {
            ret = min - ( in * ( min - max ) );
        }
        else
        {
            ret = min + ( in * ( max - min ) );
        }
        return ret;
    }
    DspFloatType linearDescale( DspFloatType in, DspFloatType min, DspFloatType max )
    {
        DspFloatType ret;
        if ( min == 0.0f && max == 0.0f )
        {
            ret = 0.0f;
        }
        else if ( min > max )
        {
            ret = ( min - in ) / ( min - max );
        }
        else
        {
            ret = ( in - min ) / ( max - min );
        }
        return ret;
    }
    DspFloatType expoScale( DspFloatType in, DspFloatType min, DspFloatType max )
    {
        // negative log makes no sense...
        if ( min < 0.0f || max < 0.0f )
        {
            return 0.0f;
        }
        // not handling min > max (inverse) case yet
        // figure out how many "octaves" (doublings) it takes to get from min to
        // max
        // we only have log & log10 so we have to do change of base
        // note this uses + instead of * so we can handle min == 0
        DspFloatType octaves = log( max - min + 1 ) / log( 2.0f );
        return ( min - 1 ) + std::pow( 2.0f, in * octaves );
    }
    DspFloatType expoDescale( DspFloatType in, DspFloatType min, DspFloatType max )
    {
        // see above
        if ( min < 0.0f || max < 0.0f )
        {
            return 0.0f;
        }
        // again, not handling min > max (inverse) case yet
        
        // note this was derived by simply inverting the previous function
        DspFloatType log2 = log( 2.0f );
        return ( log( in - min + 1 ) / log2 ) / ( log( max - min + 1 ) / log2 );
    }
    DspFloatType floorScale( DspFloatType in, DspFloatType min, DspFloatType max )
    {
        if ( min > max )
        {
            return ceil( linearScale( in, min, max ) );
        }
        else
        {
            return floor( linearScale( in, min, max ) );
        }
    }
    DspFloatType expoShape( DspFloatType in, DspFloatType amount )
    {
        if ( in == 0.0f )
            return in;
        DspFloatType flip = in < 0.0f ? -1.0f : 1.0f;
        return std::pow( in * flip, amount ) * flip;
    }

    
    DspFloatType softClipShape( DspFloatType in, DspFloatType amount )
    {
        return in / ( 1 + fabs( in ) );
    }
    DspFloatType sineShape( DspFloatType in, DspFloatType amount )
    {
        return in * cos( in * amount );
    }
    DspFloatType chebyshevRec( DspFloatType sample, int depth, DspFloatType softClipThreshold=0.9 )
    {
        if ( depth == 0 )
        {
            return 1.0f;
        }
        // lastval represents C(k-1)DspFloatType Distortion::softClip(DspFloatType sample)
        {
            if (sample < -1.f) {
                return -softClipThreshold; //2/3
            }
            else if (sample > 1.f) {
                return softClipThreshold;
            }
            else {
                return sample - ((sample * sample * sample) / 3.f);
            }
        }
    }
    
    DspFloatType chebyshevShape( DspFloatType in, DspFloatType amount )
    {
        return chebyshevRec( in, (int)amount );
    }

    // Arctangent nonlinearity
    DspFloatType arctangent(DspFloatType sample, DspFloatType alpha)
    {
        // f(x) = (2 / M_PI) * arctan(alpha * x[n]), where alpha >> 1 (drive param)
        return (2.f / M_PI)* atan(alpha * sample);
    }
    // Hard-clipping nonlinearity
    DspFloatType hardClip(DspFloatType sample)
    {
        if (sample < -1.f) {
            return -1.f;
        }
        else if (sample > 1.f) {
            return 1.f;
        }
        else {
            return sample;
        }
    }
    // Square law series expansion
    DspFloatType squareLaw(DspFloatType sample, DspFloatType alpha)
    {
        return sample + alpha * sample * sample;
    }
    DspFloatType cubicWaveShaper(DspFloatType sample)
    {
        return 1.5f * sample - 0.5f * sample * sample * sample;
    }
    

    // Foldback nonlinearity, input range: (-inf, inf)
    DspFloatType foldback(DspFloatType sample, DspFloatType threshold=0.96)
    {
        // Threshold should be > 0.f
        if (sample > threshold || sample < -threshold) {
            sample = fabs(fabs(fmod(sample - threshold,
                                    threshold * 4))
                        - threshold * 2) - threshold;
        }
        return sample;
    }
    
    // A nonlinearity by Partice Tarrabia and Bram de Jong
    DspFloatType waveShaper1(DspFloatType sample, DspFloatType alpha)
    {
        const DspFloatType k = 2.f * alpha / (1.f - alpha);
        return (1.f + k) * sample / (1.f + k * fabs(sample));
    }
    // A nonlinearity by Jon Watte
    DspFloatType waveShaper2(DspFloatType sample, DspFloatType alpha)
    {
        const DspFloatType z = M_PI * alpha;
        const DspFloatType s = 1.f / sin(z);
        const DspFloatType b = 1.f / alpha;
        
        if (sample > b) {
            return 1.f;
        }
        else {
            return sin(z * sample) * s;
        }
    }
    // A nonlinearity by Bram de Jong, input range: [-1, 1]
    DspFloatType waveShaper3(DspFloatType sample, DspFloatType alpha)
    {
        // original design requires sample be positive
        // alpha: [0, 1]
        bool isNegative = false;
        DspFloatType output = sample;
        if (sample < 0.f) {
            isNegative = true;
            output = -output;
        }
        
        if (output > alpha) {
            output = alpha + (output - alpha)
                / (1.f + std::pow(((output - alpha)) / (1.f - alpha), 2.f));
        }
        if (output > 1.f) {
            output = (alpha + 1.f) / 2.f;
        }
        
        if (isNegative) {
            output = -output;
        }
        
        return output;
    }
    
    
    DspFloatType gloubiBoulga(DspFloatType sample)
    {
        const DspFloatType x = sample * 0.686306;
        const DspFloatType a = 1 + exp(sqrt(fabs(x)) * -0.75);
        return (exp(x) - exp(-x * a)) / (exp(x) + exp(-x));
    }
    // Approximation based on description in gloubiBoulga
    DspFloatType gloubiApprox(DspFloatType sample)
    {
        return sample - (0.15f * sample * sample) - (0.15f * sample * sample * sample);
    }
    inline DspFloatType FuzzEdgeTable(const DspFloatType x)
    {
        static auto gen = std::minstd_rand(2112);
        static auto dist = std::uniform_real_distribution<DspFloatType>(-1.0, 1.0);
        auto g = x * x * x * x;
        auto xadj = 0.85 * x + 0.15 * g * dist(gen);
        return xadj;
    }
    template<typename T>
    T limitclip (T x) noexcept
    {
        return clamp(x,-0.1f, 0.1f);
    }
           
    struct table1d { // 1-dimensional function table
        DspFloatType low;
        DspFloatType high;
        DspFloatType istep;
        int size;
        DspFloatType *data;
    };
    template <int tab_size>
    struct table1d_imp {
        DspFloatType low;
        DspFloatType high;
        DspFloatType istep;
        int size;
        DspFloatType data[tab_size];
        operator table1d&() const { return *(table1d*)this; }
    };
    static table1d_imp<200> tube_table {
        0,0.880571,55,200, {
        0.000000000000,0.016750418760,0.033500837521,0.050251256281,0.067001675042,
        0.083752093802,0.100502512563,0.117252931323,0.134003350084,0.150753768844,
        0.167504187605,0.184254606365,0.201005025126,0.217755443886,0.234505862647,
        0.251256281407,0.268006700168,0.284757118928,0.301507537688,0.318257956449,
        0.335008375209,0.351758793970,0.368509212730,0.385259631491,0.402010050251,
        0.418760469012,0.435510887772,0.452261306533,0.469011725293,0.485762144054,
        0.502512562814,0.519262981575,0.536013400335,0.552763819095,0.569514237856,
        0.586264656616,0.593999365915,0.599005700058,0.603333878092,0.607267980657,
        0.610930403272,0.614388585430,0.617684834322,0.620847956424,0.623898713479,
        0.626852710073,0.629722059392,0.632516408598,0.635243602944,0.637910133907,
        0.640521451968,0.643082191206,0.645596334502,0.648067337630,0.650498224157,
        0.652891659201,0.655250007565,0.657575380145,0.659869671394,0.662134589886,
        0.664371683478,0.666582360199,0.668767905735,0.670929498165,0.673068220463,
        0.675185071166,0.677280973536,0.679356783465,0.681413296334,0.683451252980,
        0.685471344926,0.687474218971,0.689460481240,0.691430700768,0.693385412682,
        0.695325121033,0.697250301337,0.699161402840,0.701058850561,0.702943047129,
        0.704814374447,0.706673195193,0.708519854182,0.710354679615,0.712177984199,
        0.713990066189,0.715791210325,0.717581688703,0.719361761564,0.721131678031,
        0.722891676777,0.724641986646,0.726382827231,0.728114409400,0.729836935788,
        0.731550601256,0.733255593312,0.734952092506,0.736640272797,0.738320301896,
        0.739992341580,0.741656547999,0.743313071948,0.744962059131,0.746603650406,
        0.748237982013,0.749865185791,0.751485389382,0.753098716418,0.754705286704,
        0.756305216386,0.757898618110,0.759485601172,0.761066271663,0.762640732600,
        0.764209084056,0.765771423280,0.767327844807,0.768878440572,0.770423300008,
        0.771962510148,0.773496155712,0.775024319196,0.776547080961,0.778064519305,
        0.779576710544,0.781083729079,0.782585647471,0.784082536500,0.785574465234,
        0.787061501083,0.788543709858,0.790021155826,0.791493901762,0.792962008997,
        0.794425537468,0.795884545762,0.797339091160,0.798789229679,0.800235016113,
        0.801676504067,0.803113746001,0.804546793259,0.805975696107,0.807400503763,
        0.808821264430,0.810238025325,0.811650832709,0.813059731915,0.814464767373,
        0.815865982638,0.817263420412,0.818657122571,0.820047130187,0.821433483549,
        0.822816222186,0.824195384887,0.825571009720,0.826943134054,0.828311794573,
        0.829677027299,0.831038867606,0.832397350237,0.833752509322,0.835104378390,
        0.836452990391,0.837798377701,0.839140572145,0.840479605004,0.841815507032,
        0.843148308470,0.844478039054,0.845804728029,0.847128404163,0.848449095754,
        0.849766830645,0.851081636231,0.852393539473,0.853702566904,0.855008744674,
        0.856312098429,0.857612653515,0.858910434856,0.860205466994,0.861497774101,
        0.862787379985,0.864074308096,0.865358581537,0.866640223071,0.867919255124,
        0.869195699798,0.870469578875,0.871740913821,0.873009725798,0.874276035666,
        0.875539863991,0.876801231052,0.878060156841,0.879316661078,0.880570763209
        }
    };
    DspFloatType tubeclip(DspFloatType x) {
        DspFloatType f = fabs(x);
        f = f * tube_table.istep;
        int i = static_cast<int>(f);
        if (i < 0) {
            f = tube_table.data[0];
        } else if (i >= tube_table.size-1) {
            f = tube_table.data[tube_table.size-1];
        } else {
        f -= i;
        f = tube_table.data[i]*(1-f) + tube_table.data[i+1]*f;
        }
        return copysign(f, x);
    }
    

    DspFloatType hardClipping(const DspFloatType& _in)
    {
        DspFloatType out;
        DspFloatType threshold = 0.5f;
        
        if (_in > threshold)
            out = threshold;
        else if (_in < -threshold)
            out = -threshold;
        else
            out = _in;
        
        return out;
    }
    DspFloatType softClipping(const DspFloatType& _in)
    {
        DspFloatType out;
        DspFloatType threshold1 = 1.0f / 3.0f;
        DspFloatType threshold2 = 2.0f / 3.0f;
        
        if (_in > threshold2)
            out = 1.0f;
        else if (_in > threshold1)
            out = 1.0f - std::pow (2.0f - 3.0f * _in, 2.0f) / 3.0f;
        else if (_in < -threshold2)
            out = -1.0f;
        else if (_in < -threshold1)
            out = -1.0f + std::pow (2.0f + 3.0f * _in, 2.0f) / 3.0f;
        else
            out = 2.0f * _in;
            out *= 0.5f;
        
        return out;
    }
    
    DspFloatType exponential(const DspFloatType& _in)
    {
        DspFloatType out;
        
        if (_in > 0.0f)
            out = 1.0f - expf (-_in);
        else
            out = -1.0f + expf (_in);
        
        return out;
    }
    
    DspFloatType fullWaveRectifier(const DspFloatType& _in)
    {
        DspFloatType out;
        
        out = fabs (_in);
        
        return out;
    }
    DspFloatType halfWaveRectifier(const DspFloatType& _in)
    {
        DspFloatType out;
        
        if (_in > 0.0f)
            out = _in;
        else
            out = 0.0f;
        
        return out;
        
    }
    

    DspFloatType ArayaAndSuyama(const DspFloatType &_in)
    {
        DspFloatType out;
        
        out = (3/2) * (_in) * (1 - std::pow(_in, 2)/3);
        out = (3/2) * (out) * (1 - std::pow(out, 2)/3);
        out = (3/2) * (out) * (1 - std::pow(out, 2)/3);
        return out;
        
    }
    DspFloatType doidicSymmetric(const DspFloatType& _in)
    {
        DspFloatType out;
        
        out =  ( (2*fabs(_in))  - std::pow(_in, 2)) * copysignf(1, _in);
        
        return out;
    }
    
    DspFloatType doidicAssymetric(const DspFloatType& _in)
    {
        DspFloatType out;
        
        if (_in >= -1 && _in < -0.08905f) {
            out = -(0.75)*( 1 - (1 - std::pow(std::fabs(_in) - 0.032847, 12)) + 1/3*(std::fabs(_in) - 0.032847)) + 0.01;
        }
        else if (_in >= -0.08905f && _in < 0.320018)
        {
            out = -6.153 * std::pow(_in,2) + 3.9375 * _in;
        }
        else if (_in >= 0.320018 && _in <= 1)
        {
            out = 0.630035;
        }
        
        return out;
        
    }

    
}

    

