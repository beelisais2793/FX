#pragma once

#include <cmath>
#include "ClipFunctions.hpp"

namespace FX::Distortion
{

    // First two functions from here:
    // http://www.mitpressjournals.org/doi/pdf/10.1162/comj.2009.33.2.85
    DspFloatType distortionOne(DspFloatType input)
    {
        return 3 * input / 2 * ((1 - pow(input, 2) / 3));
    }


    DspFloatType distortionTwo(DspFloatType input)
    {
        // if input > 0 sign is minus
        const DspFloatType sign = input > 0 ? 1.0f : -1.0f;
        
        return sign * (fabs(2 * input) - pow(input, 2));
    }



    // Functions from MusicDSP: http://musicdsp.org



    /*
    Soft saturation: http://musicdsp.org/showone.php?id=42
    
    Type : waveshaper
    References : Posted by Bram de Jong
    
    Notes :
    This only works for positive values of x. param should be in the range 0..1
    */
    DspFloatType softSaturation(DspFloatType input, DspFloatType param)
    {
        DspFloatType output;
        
        if (input < param)
        {
            output = input;
        }

        if (input > param)
        {
            output = param + (input - param) / (1 + pow(((input - param) / (1 - param)), 2));
        }


        if (input > 1)
        {
            output = (param + 1) / 2;
        }
        
        return output;
    }


    // Overdrive, default drive = 0
    DspFloatType overdrive2(DspFloatType input, DspFloatType drive = 1.0f)
    {
        return (((sqrt(input) - input) * drive) + input) * (1 - ((drive / 4) * 0.5));
    }


    // Clipping, not very cool: http://musicdsp.org/showone.php?id=104
    DspFloatType clipper(DspFloatType input, DspFloatType drive)
    {
        DspFloatType sign = (input < 0) ? -1.0f : 1.0f;
        
        return sign * pow(atan (pow (fabs(input), drive)), (1 / drive));
    }    
    // distortions
    inline DspFloatType arctanDistortion(DspFloatType x, DspFloatType g = 1) {
        return 2/M_PI * std::atan(x*g);
    }
    // triangle = -asin(sin(2*pi*x))/1.5
    // triangle = acos(sin(2*pi*x))/3
    inline DspFloatType triangleDistortion1(DspFloatType x, DspFloatType g=1) {
        return -asin(2*M_PI*x*g)/1.5;
    }
    inline DspFloatType triangleDistortion2(DspFloatType x, DspFloatType g=1) {
        return acos(sin(2*M_PI*x*g))/3.0;
    }
    inline DspFloatType asinDistortion(DspFloatType x, DspFloatType g=1) {
        return std::asin(x*g)/1.57;
    }
    inline DspFloatType acosDistortion(DspFloatType x, DspFloatType g=1) {
        return 2*(-std::acos(x*g)/3+1)-1;
    }
    inline DspFloatType asinhDistortion(DspFloatType x, DspFloatType g=1)
    {
        return std::asinh(g*sin(2*M_PI*x*g))/(g/2.0f);
    }
    /*
    inline sample_vector<DspFloatType> arctanDistortion(sample_vector<DspFloatType> x, DspFloatType g = 1) {
        return 2/M_PI * atan(x*g);
    }
    */
    inline DspFloatType assymetricDistortion(DspFloatType x, DspFloatType dc = 0.1) {
        DspFloatType in = x + dc;
        if(std::abs(in) > 1) in = signum(in);
        DspFloatType k = (in - (1.0f/5.0f)*std::pow(in,5));
        return k - dc;
    }
    inline DspFloatType squashedlog(DspFloatType x, DspFloatType g = 1,DspFloatType h=1) {
        return std::tanh(h*signum(x)*std::log(1+std::abs(g*x))/std::log(2));
    }
    inline DspFloatType wickedlog(DspFloatType x, DspFloatType g = 1,DspFloatType h=1) {
        return std::tanh(h*signum(x)*std::log(1.2+2*std::abs(g*x))/std::log(2));
    }

    DspFloatType logarithmic(DspFloatType x)          { return std::log(1 + x) / std::log(2); }
    DspFloatType soft(DspFloatType x, DspFloatType g =1)     { return clamp(2.0f/(1+std::exp(-g*x)) - 0.5f,-1,1);}
    DspFloatType exp1(DspFloatType x,DspFloatType gain=1)    { return clamp(std::exp(gain*x)/std::exp(gain)-1,-1,1); }
    DspFloatType iexp(DspFloatType x,DspFloatType gain=1)    { return clamp((std::exp(gain*x)-1)/std::exp(gain)-1,-1,1); }
    DspFloatType exp2(DspFloatType x,DspFloatType gain=1)    { return clamp(-(std::exp(x)-1)/(1+std::exp(gain*x))*(1-std::exp(x)),-1,1); }
    DspFloatType logi(DspFloatType x,DspFloatType gain=1)    { return clamp(std::log(std::abs(gain)*std::abs(gain*x))/std::log(std::abs(gain)),-1,1);}
    DspFloatType ilogi(DspFloatType x,DspFloatType gain=1)   { return clamp(std::log(std::abs(gain*x)-1/(std::log(std::abs(gain)))),-1,1);}
    DspFloatType softlog(DspFloatType x,DspFloatType gain=1) { return clamp(-(std::exp(std::abs(x*gain))-1)/(1+std::exp(std::abs(gain)))*(std::log(std::abs(x/3.6))),-1,1); }           
    DspFloatType curvey(DspFloatType x,DspFloatType g=1)     { return clamp(x/(1.0/cos(atan(5*g*x))*0.2),-1,1); }
    DspFloatType curv2(DspFloatType x,DspFloatType g=1)      { return clamp(2*(x/(std::atanh(g*x))*-1+1),-1,1); }
    DspFloatType erf(DspFloatType x,DspFloatType g=1)        { return clamp(1.5*std::erf(g*x),-1,1); }
    DspFloatType fasterf(DspFloatType x,DspFloatType g=1)    { return clamp(2*(1+std::tanh(1.5*std::erf(g*x)))-1,-1,1); }
    DspFloatType gd2(DspFloatType x,DspFloatType g=1)        { return clamp(2*(x/(1+std::abs(g*x))),-1,1); }
    DspFloatType tan2(DspFloatType x,DspFloatType g=1)       { return clamp(2*(2/M_PI*std::tan((2/M_PI)*g*x)),-1,1); }
    DspFloatType atan(DspFloatType x,DspFloatType g=1)       { return clamp(3*(2/M_PI*std::atan((2/M_PI)*g*x)),-1,1); }
    DspFloatType asymsoft(DspFloatType x,DspFloatType g=1)   { return clamp(std::tanh(1.5*std::erf(-g*x)) + 0.5f,-1,1);}
    DspFloatType sigmoid(DspFloatType x, DspFloatType g = 10.0f) { return clamp(1 / (1 + exp(-g*x)),-1,1); }

    DspFloatType halfwave(DspFloatType x) {
        if(x > 0) return x;
        return 0;
    }
    DspFloatType fullwave(DspFloatType x) {
        if(x > 0) return x;
        return -x;
    }

    DspFloatType saturate(DspFloatType x, DspFloatType t, DspFloatType g)
    {
        if(fabs(x)<t)
            return x;
        else
        {
            if(x > 0.f)
                return t + (1.f-t)*tanh(g*((x-t)/(1-t)));
            else
                return -(t + (1.f-t)*tanh(g*((-x-t)/(1-t))));
        }
    }


    ///////////////////////////////////////////////////////////////
    // Distortion
    ///////////////////////////////////////////////////////////////
    DspFloatType overdrive(DspFloatType x, DspFloatType thresh=0.333)
    {
        DspFloatType a = std::abs(x);
        if( a < thresh) return 2*x;
        if( a >= thresh) {
            DspFloatType r = (3-(2-x*3)*(2-x*3))/3;
            if(x > 0) return r;
            else return -r;
        }
        if(x > 0) return 1;
        return -1;
    }
    DspFloatType fuzz(DspFloatType x, DspFloatType a, DspFloatType mix = 0.5)
    {
        DspFloatType t = std::abs(x);
        if(t == 0) return 0;
        DspFloatType r = (x/t)*(1-std::exp(a*(x*x)/t));
        return mix*x + (1.0-mix)*r;
    }
    DspFloatType sigmoid_fuzz(DspFloatType x, DspFloatType a, DspFloatType mix = 0.5)
    {
        DspFloatType t = std::abs(x);
        DspFloatType s = sigmoider(x);
        DspFloatType r = (s/t)*(1-std::exp(a*(s*s)/t));
        return mix*x + (1.0-mix)*r;
    }
    DspFloatType tanh_fuzz(DspFloatType x, DspFloatType a, DspFloatType mix = 0.5)
    {
        DspFloatType t = std::abs(x);
        DspFloatType s = tanhmoider(x,a);
        DspFloatType r = (s/t)*(1-std::exp(a*(s*s)/t));
        return mix*x + (1.0-mix)*r;
    }

// bit  = 1..16
// rate = decimation rate
// coef = bits * aaten * 8/5.0
// bits = bit - coeff
/*
DspFloatType BitCrusher_decimate(DspFloatType i, long int bits, DspFloatType rate)
{



	long int m = 1 << (bits - 1);
	cnt += rate;
	if (cnt >= 1)
	{
		cnt -= 1;
		y = (long int)(i * m) / (DspFloatType)m;
	}
	return y;
}
*/

DspFloatType dafe_foldback(DspFloatType in, DspFloatType threshold)
{
	if (in>threshold || in<-threshold)
	{
		in = fabs(fabs(fmod(in - threshold, threshold * 4)) - threshold * 2) - threshold;
	}
	return in;
}


}