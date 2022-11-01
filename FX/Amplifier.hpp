#pragma once

#include "SoundObject.hpp"
#include "DiodeClipper.hpp"
#include "Diode.hpp"
#include "Amplifiers.hpp"
#include "DistortionFunctions.hpp"

namespace FX::Distortion::Amplifier
{

    DspFloatType function_morpher(DspFloatType x, DspFloatType y, DspFloatType frac=0.5)
    {
        return (1.0-frac)*x + frac*y;
    }

    enum {
        DISTORTION_NONE,
       
        DISTORTION_DC,
        DISTORTION_BIPOLAR,
        DISTORTION_QUADRATIC,
        DISTORTION_QUADRATIC2,
        DISTORTION_QUADRATIC3,
        DISTORTION_PARAMETRIC,
        DISTORTION_ARCTAN2,
        DISTORTION_SOFT,
        DISTORTION_ERF,
        DISTORTION_HARDCLIP,
        DISTORTION_HYPERTAN,
        DISTORTION_DIODECLIP,
        DISTORTION_FUZZEXP,
        DISTORTION_PIECEWISE,
        DISTORTION_TUBE,
        DISTORTION_ARRAYA,
        DISTORTION_GALLO,
        DISTORTION_DOUBLESOFT,
        DISTORTION_CRUSH,
        DISTORTION_TUBOID,
        DISTORTION_YEH, 
        
        DISTORTION_FULLRECTIFY,
        DISTORTION_HALFRECTIFY,
        
        DISTORTION_ATAN,
        DISTORTION_EXP,
        
        DISTORTION_ASIGMOID,
        DISTORTION_ASIGMOID2,
        DISTORTION_DISTORTION,
        DISTORTION_CUBIC,
        
        
        DISTORTION_CHEBY2,
        DISTORTION_CHEBY3,
        DISTORTION_CHEBY4,
        DISTORTION_CHEBY5,
        DISTORTION_CHEBY6,
        DISTORTION_CHEBY7,
        DISTORTION_CHEBY8,
        DISTORTION_CHEBY9,        
        
        DISTORTION_ARCTAN,  
        DISTORTION_CLIPPER,
        
        DISTORTION_ONE,
        DISTORTION_TWO,
        DISTORTION_OVERDRIVE,
        DISTORTION_OVERDRIVE2,
        
        DISTORTION_ASYMETRIC,
        DISTORTION_SQUASHEDLOG,
        DISTORTION_WICKEDLOG,
        DISTORTION_HALFWAVE,
        DISTORTION_FULLWAVE,
        DISTORTION_SATURATE,
        DISTORTION_FUZZ,
        DISTORTION_SIGMOID_FUZZ,
        DISTORTION_TANH_FUZZ,

        SIGMOID_SIGMOID_FUNCTION,    
        SIGMOID_SERPENT_CURVE,    
        SIGMOID_SIGMOIDER,
        SIGMOID_ERFMOIDER,
        SIGMOID_GUNDERBALLS,
        SIGMOID_ALGEBRABALLS,
        SIGMOID_ALGEBRAMOIDER,    
        SIGMOID_TANHIFY,
        SIGMOID_TANHMOIDER,
        SIGMOID_TANHBALLS,
        SIGMOID_ATANBALLS,
        SIGMOID_ATANMOIDER,

        DISTORTION_UDO1,
        DISTORTION_FOLD,
        DISTORTION_WRAP,
        DISTORTION_SINFOLD,
        DISTORTION_DIODE,
        DISTORTION_TANH,
        DISTORTION_TANH_NORMAL,
        DISTORTION_POSITIVE,
        DISTORTION_NEGATIVE,    
        DISTORTION_SIGMOID,
        DISTORTION_SIGMOID_MINUS,
        DISTORTION_BPSIGMOID,        
        DISTORTION_LAST,       
        // doesn't work??
        DISTORTION_ASIN,
        DISTORTION_ASINH,
        DISTORTION_ACOS,    
        DISTORTION_TRI1,
        DISTORTION_TRI2,
        DISTORTION_ASIN2,
        DISTORTION_ACOS2,
        DISTORTION_ASINH2,
        DISTORTION_ACOSH,
        DISTORTION_ATANH,


    };

    struct Amplifier : public AmplifierProcessor
    {
        // ToneStack
        DspFloatType G;
        DspFloatType bias=0;
        DspFloatType prv;
        DspFloatType min = -1;
        DspFloatType max = 1;
        DspFloatType postgain = 1;
        DspFloatType pregain = 1;
        DspFloatType sat_threshold = 0.95;
        DspFloatType fuzz_mix = 0.5;
        
        int dist1_clip = DISTORTION_TANH;
        int dist2_clip = DISTORTION_SIGMOID;
        DspFloatType distortion_morph = 0.5;

        
        DspFloatType preGain = 1.0f;
        DspFloatType postGain = 1.0f;

        enum {
            PORT_G,
            PORT_BIAS,
            PORT_MIN,
            PORT_MAX,
            PORT_POSTGAIN,
            PORT_PREGAIN,
            PORT_SATTHRSH,
            PORT_FUZZ,
            PORT_DIST1,
            PORT_DIST2,
            PORT_DISTMORPH,
        };
        void printPorts() {
            printf("setPort\n");
            printf("PORT_G=0\n");
            printf("PORT_BIAS=1\n");
            printf("PORT_MIN=2\n");
            printf("PORT_MAX=3\n");
            printf("PORT_POSTGAIN=4\n");
            printf("PORT_PREGAIN=5\n");
            printf("PORT_SATTHRSH=6\n");
            printf("PORT_FUZZ=7\n");
            printf("PORT_DIST1=8\n");
            printf("PORT_DIST2=9\n");
            printf("PORT_DISTMORPH=10\n");
        }
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case PORT_G: G = v; break;
                case PORT_BIAS: bias = v; break;
                case PORT_MIN: min = v; break;
                case PORT_MAX: max = v; break;
                case PORT_POSTGAIN: postgain = postGain = v; break;
                case PORT_PREGAIN: pregain = preGain = v; break;
                case PORT_SATTHRSH: sat_threshold = v; break;
                case PORT_FUZZ: fuzz_mix = v; break;
                case PORT_DIST1: dist1_clip = (int)v % DISTORTION_LAST;
                case PORT_DIST2: dist2_clip = (int)v % DISTORTION_LAST;
                case PORT_DISTMORPH: distortion_morph = v; break;
                default: printf("No port %d\n",port);
            }
        }
        Amplifier(DspFloatType Gain = 1.0f, DspFloatType b = 0.0f)
        : AmplifierProcessor()
        {
            G = Gain;
            bias = b;       
            RandomClip(); 
        }
        
        virtual void RandomClip() 
        {
            fuzz_mix = noise.rand();
            sat_threshold = 0.5 + 0.5*noise.rand();
            postGain = pow(10,noise.randint(0,12)/20.0f);
            preGain = pow(10,noise.randint(0,12)/20.0f);
            min = -0.95 + -0.05*noise.rand();
            max = 0.95+0.05*noise.rand();
            bias = 0.01*noise.rand();
            dist1_clip = noise.randint(1,DISTORTION_LAST);
            dist2_clip = noise.randint(1,DISTORTION_LAST);
            printf("%d %d\n",dist1_clip,dist2_clip);
            distortion_morph = noise.rand();
        }
        DspFloatType Integrator(DspFloatType in) {
            DspFloatType r = in + prv;
            prv = in;
            return r;
        }
        DspFloatType Differencer(DspFloatType in) {
            DspFloatType r = in - prv;
            prv = in;
            return r;
        }
        void SetBias(DspFloatType b) {
            bias = b;
        }

        DspFloatType distortionClip(int dist_clip, DspFloatType I, DspFloatType K)
        {
            DspFloatType A = G;
            DspFloatType X = min;
            DspFloatType Y = max;
            
            switch(dist_clip)   
            {
                case DISTORTION_DIODE: return Distortion::Diode::Diode(I);
                case DISTORTION_TANH: return std::tanh(I); 
                case DISTORTION_TANH_NORMAL: return Distortion::tanh_normal(I,K); break;
                case DISTORTION_ONE: return Distortion::distortionOne(I);
                case DISTORTION_TWO: return Distortion::distortionTwo(I);
                case DISTORTION_OVERDRIVE: return Distortion::overdrive(I,K);
                case DISTORTION_OVERDRIVE2: return Distortion::overdrive2(I,K);
                case DISTORTION_CLIPPER: return Distortion::clipper(I,K);
                case DISTORTION_ARCTAN: return Distortion::arctanDistortion(I,K);
                case DISTORTION_TRI1: return Distortion::triangleDistortion1(I);
                case DISTORTION_TRI2: return Distortion::triangleDistortion2(I);
                case DISTORTION_ASIN: return Distortion::asinDistortion(I);
                case DISTORTION_ACOS: return Distortion::acosDistortion(I);
                case DISTORTION_ASINH: return Distortion::asinhDistortion(I,K);
                case DISTORTION_ASYMETRIC: return Distortion::asymmetricSigmoidDistortionFunction(I);
                case DISTORTION_SQUASHEDLOG: return Distortion::squashedlog(I,K);
                case DISTORTION_WICKEDLOG: return Distortion::wickedlog(I,K);
                case DISTORTION_HALFWAVE: return Distortion::halfwave(I);
                case DISTORTION_FULLWAVE: return Distortion::fullwave(I);
                case DISTORTION_SATURATE: return Distortion::saturate(I,sat_threshold,K);
                case DISTORTION_FUZZ: return Distortion::fuzz(I,K,fuzz_mix);
                case DISTORTION_SIGMOID_FUZZ: return Distortion::sigmoid_fuzz(I,K,fuzz_mix);
                case DISTORTION_TANH_FUZZ: return Distortion::tanh_fuzz(I,K,fuzz_mix);

                case DISTORTION_UDO1: return Distortion::udo1(I,K);
                case DISTORTION_FOLD: return Distortion::Fold(I);
                case DISTORTION_WRAP: return Distortion::Wrap(I);
                case DISTORTION_SINFOLD: return Distortion::SinFold(I);
                case DISTORTION_POSITIVE: return Distortion::positive_signal(I);
                case DISTORTION_NEGATIVE: return Distortion::negative_signal(I);
                //case DISTORTION_SIGMOID: return sigmoid_function(I,K);
                case DISTORTION_SIGMOID_MINUS: return Distortion::sigmoid_minus(I,K);
                case DISTORTION_BPSIGMOID: return Distortion::bpsigmoid(I,K);
                case DISTORTION_FULLRECTIFY: return Distortion::full_rectify(I);
                case DISTORTION_HALFRECTIFY: return Distortion::half_rectify(I);
                
                case DISTORTION_ASIGMOID: return Distortion::asymmetricSigmoidDistortionFunction(I);
                case DISTORTION_ASIGMOID2: return Distortion::asymmetricSigmoidDistortionFunction2(I);            
                case DISTORTION_DISTORTION: return Distortion::distortionFunction(I);
                case DISTORTION_ARCTAN2: return Distortion::arcTanDistortion(I,A,X,Y);
                case DISTORTION_SOFT: return Distortion::softClipper(I,A,X,Y);
                case DISTORTION_ERF: return Distortion::errorf(I,A,X,Y);
                case DISTORTION_SIGMOID: return Distortion::sigmoided(I,A,X,Y);
                case DISTORTION_HARDCLIP: return Distortion::hardclip(I,A,X,Y);
                case DISTORTION_HYPERTAN: return Distortion::hyperbolicTangent(I,A,X,Y);
                case DISTORTION_DIODECLIP: return Distortion::diodeClipping(I,A,X,Y);
                case DISTORTION_FUZZEXP: return Distortion::fuzzExponential(I,A,X,Y);
                case DISTORTION_PIECEWISE: return Distortion::pieceWiseOverdrive(I,A,X,Y);
                case DISTORTION_TUBE: return Distortion::tube(I,A,X,Y);
                case DISTORTION_ARRAYA: return Distortion::arraya(I,A,X,Y);
                case DISTORTION_GALLO: return Distortion::gallo(I,A,X,Y);
                case DISTORTION_DOUBLESOFT: return Distortion::doubleSoftClipper(I,A,X,Y);
                case DISTORTION_CRUSH: return Distortion::crush(I,A,X,Y);
                case DISTORTION_TUBOID: return Distortion::tuboid(I,A,X,Y);
                case DISTORTION_YEH: return Distortion::pakarinen_Yeh(I,A,X,Y);

                case DISTORTION_CUBIC: return Distortion::cubic_distortion(I,A,X,Y);
                case DISTORTION_ASIN2: return Distortion::asin_distortion(I,A,X,Y);
                case DISTORTION_ACOS2: return Distortion::acos_distortion(I,A,X,Y);
                case DISTORTION_ATAN: return Distortion::atan_distortion(I,A,X,Y);
                case DISTORTION_ASINH2: return Distortion::asinh_distortion(I,A,X,Y);
                case DISTORTION_ACOSH: return Distortion::acosh_distortion(I,A,X,Y);
                case DISTORTION_ATANH: return Distortion::atanh_distortion(I,A,X,Y);
                case DISTORTION_EXP: return Distortion::exp_distortion(I,A,X,Y);
                case DISTORTION_DC: return Distortion::dc_distortion(I,A,X,Y);
                case DISTORTION_BIPOLAR: return Distortion::bipolar_distortion(I,A,X,Y);
                case DISTORTION_QUADRATIC: return Distortion::quadratic_distortion(I,A,X,Y);
                case DISTORTION_QUADRATIC2: return Distortion::quadratic2_distortion(I,A,X,Y);
                case DISTORTION_QUADRATIC3: return Distortion::quadratic3_distortion(I,A,X,Y);
                case DISTORTION_PARAMETRIC: return Distortion::parametric_clip(I,A,X,Y);

                case SIGMOID_SERPENT_CURVE: return Distortion::serpent_curve(I,K); break;
                case SIGMOID_SIGMOID_FUNCTION: return Distortion::sigmoid_function(I,K); break;
                case SIGMOID_SIGMOIDER: return Distortion::sigmoider(I,K); break;
                case SIGMOID_ERFMOIDER: return Distortion::erfmoider(I,K); break;
                case SIGMOID_GUNDERBALLS: return Distortion::gunderballs(I,K); break;
                case SIGMOID_ALGEBRABALLS: return Distortion::algebraballs(I,K); break;
                case SIGMOID_ALGEBRAMOIDER: return Distortion::algebramoider(I,K); break;
                case SIGMOID_TANHIFY: return Distortion::tanhify(I,K); break;
                case SIGMOID_TANHMOIDER: return Distortion::tanhmoider(I,K); break;
                case SIGMOID_TANHBALLS: return Distortion::tanhballs(I,K); break;
                case SIGMOID_ATANBALLS: return Distortion::atanballs(I,K); break;
                case SIGMOID_ATANMOIDER: return Distortion::atanmoider(I,K); break;
                
                case DISTORTION_CHEBY2: return Distortion::cheby(2,I); break;
                case DISTORTION_CHEBY3: return Distortion::cheby(3,I); break;
                case DISTORTION_CHEBY4: return Distortion::cheby(4,I); break;
                case DISTORTION_CHEBY5: return Distortion::cheby(5,I); break;
                case DISTORTION_CHEBY6: return Distortion::cheby(6,I); break;            
                case DISTORTION_CHEBY7: return Distortion::cheby(7,I); break;
                case DISTORTION_CHEBY8: return Distortion::cheby(8,I); break;
                case DISTORTION_CHEBY9: return Distortion::cheby(9,I); break;


            }
            return I;
        }
        
        virtual DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = -1, DspFloatType Y = 1)
        {
            
            DspFloatType K = G;
            
            I += bias;
            I *= preGain;
            DspFloatType d1 = distortionClip(dist1_clip,I,K);
            DspFloatType d2 = distortionClip(dist2_clip,I,K);
            
            DspFloatType distortion = A*function_morpher(d1,d2,distortion_morph);        
            DspFloatType r = clamp(distortion,min*-X,max*Y);
            
            r -= bias;
            return tanh(postGain*r);

        }
    };

    struct TwinAmplifier : public Amplifier
    {
        Amplifier amp[2];
        //Dioder diode;

        TwinAmplifier() : Amplifier()
        {

        }
        DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = -1, DspFloatType Y = 1, DspFloatType B=0)
        {
            
            DspFloatType K = G;
            DspFloatType bt = bias;
            bias = B;
            I += bias;
            I *= preGain;
            //I = diode.Tick(I);
            DspFloatType d1 = distortionClip(dist1_clip,I,K*X);
            DspFloatType d2 = distortionClip(dist2_clip,I,K*Y);
            
            DspFloatType distortion = A*function_morpher(amp[0].Tick(d1),amp[1].Tick(d2),distortion_morph);        
            DspFloatType r = clamp(distortion,min,max);
            bias = bt;
            r -= bias;        
            return tanh(postGain*r);

        }
    };


    struct RangeAmplifier : public Amplifier
    {
        RangeAmplifier() : Amplifier()
        {

        }

        virtual DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = -1, DspFloatType Y = 1, DspFloatType B=0)
        {
            
            DspFloatType K = G;
            DspFloatType bt = bias;
            bias = B;
            I += bias;
            I *= preGain;
            DspFloatType d1 = distortionClip(dist1_clip,I,K);
            DspFloatType d2 = distortionClip(dist2_clip,I,K);
            DspFloatType distortion;
            if(fabs(I) < 0.5) distortion = d1;
            else distortion = d2;
            distortion = A*distortion;
            DspFloatType r = clamp(distortion,min*-X,max*Y);
            bias = bt;
            r -= bias;
            return tanh(postGain*r);

        }
    };

    struct Range4Amplifier : public Amplifier
    {
        int dist3_clip = DISTORTION_TANH;
        int dist4_clip = DISTORTION_SIGMOID;

        Range4Amplifier() : Amplifier()
        {

        }

        virtual void RandomClip() 
        {
            fuzz_mix = noise.rand();
            sat_threshold = 0.5 + 0.5*noise.rand();
            postGain = pow(10,noise.randint(0,12)/20.0f);
            preGain = pow(10,noise.randint(0,12)/20.0f);
            min = -0.95 + -0.05*noise.rand();
            max = 0.95+0.05*noise.rand();
            bias = 0.01*noise.rand();
            dist1_clip = noise.randint(1,DISTORTION_LAST);
            dist2_clip = noise.randint(1,DISTORTION_LAST);
            dist3_clip = noise.randint(1,DISTORTION_LAST);
            dist4_clip = noise.randint(1,DISTORTION_LAST);
            printf("%d %d\n",dist1_clip,dist2_clip);
            distortion_morph = noise.rand();
        }

        virtual DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = -1, DspFloatType Y = 1, DspFloatType B=0)
        {
            
            DspFloatType K = G;
            DspFloatType bt = bias;
            bias = B;
            I += bias;
            I *= preGain;
            
            DspFloatType d1 = distortionClip(dist1_clip,I,K);
            DspFloatType d2 = distortionClip(dist2_clip,I,K);
            DspFloatType d3 = distortionClip(dist3_clip,I,K);
            DspFloatType d4 = distortionClip(dist4_clip,I,K);

            DspFloatType distortion;
            if(fabs(I) < 0.25) distortion = d1;
            else if(fabs(I) < 0.5) distortion = d1;
            else if(fabs(I) < 0.75) distortion = d3;
            else distortion = d4;

            distortion = A*distortion;
            DspFloatType r = clamp(distortion,min*-X,max*Y);
            bias = bt;
            r -= bias;
            return tanh(postGain*r);

        }
    };

    struct QuadAmplifier : public Amplifier
    {
        int dist3_clip = DISTORTION_TANH;
        int dist4_clip = DISTORTION_SIGMOID;

        QuadAmplifier() : Amplifier()
        {

        }

        virtual void RandomClip() 
        {
            fuzz_mix = noise.rand();
            sat_threshold = 0.5 + 0.5*noise.rand();
            postGain = pow(10,noise.randint(0,12)/20.0f);
            preGain = pow(10,noise.randint(0,12)/20.0f);
            min = -0.95 + -0.05*noise.rand();
            max = 0.95+0.05*noise.rand();
            bias = 0.01*noise.rand();
            dist1_clip = noise.randint(1,DISTORTION_LAST);
            dist2_clip = noise.randint(1,DISTORTION_LAST);
            dist3_clip = noise.randint(1,DISTORTION_LAST);
            dist4_clip = noise.randint(1,DISTORTION_LAST);
            printf("%d %d\n",dist1_clip,dist2_clip);
            distortion_morph = noise.rand();
        }

        virtual DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = -1, DspFloatType Y = 1, DspFloatType B=0)
        {
            
            DspFloatType K = G;
            DspFloatType bt = bias;
            bias = B;
            I += bias;
            I *= preGain;
            
            DspFloatType d1 = distortionClip(dist1_clip,I,K);
            DspFloatType d2 = distortionClip(dist2_clip,I,K);
            DspFloatType d3 = distortionClip(dist3_clip,I,K);
            DspFloatType d4 = distortionClip(dist4_clip,I,K);

            DspFloatType theta = atan2(X,Y);
            DspFloatType distortion;
            if(theta < M_PI/2) distortion = d1;
            else if(theta < M_PI) distortion = d1;
            else if(theta < 3*M_PI/4) distortion = d3;
            else distortion = d4;

            distortion = A*distortion;
            DspFloatType r = clamp(distortion,min,max);
            bias = bt;
            r -= bias;
            return tanh(postGain*r);

        }
    };

    struct BipolarAmplifier : public Amplifier
    {
        BipolarAmplifier() : Amplifier()
        {

        }

        virtual DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = -1, DspFloatType Y = 1, DspFloatType B=0)
        {
            
            DspFloatType K = G;
            DspFloatType bt = bias;
            bias = B;
            I += bias;
            I *= preGain;
            DspFloatType d1 = distortionClip(dist1_clip,I,K);
            DspFloatType d2 = distortionClip(dist2_clip,I,K);
            
            DspFloatType distortion;
            if(I >= 0) distortion = d1;
            else distortion = d2;

            DspFloatType r = clamp(A*distortion,min*-X,max*Y);
            bias = bt;
            r -= bias;
            return tanh(postGain*r);

        }
    };

    struct BipolarAmplifier2 : public Amplifier
    {
        int dist3_clip = DISTORTION_TANH;
        int dist4_clip = DISTORTION_SIGMOID;

        BipolarAmplifier2() : Amplifier()
        {

        }

        virtual void RandomClip() 
        {
            fuzz_mix = noise.rand();
            sat_threshold = 0.5 + 0.5*noise.rand();
            postGain = pow(10,noise.randint(0,12)/20.0f);
            preGain = pow(10,noise.randint(0,12)/20.0f);
            min = -0.95 + -0.05*noise.rand();
            max = 0.95+0.05*noise.rand();
            bias = 0.01*noise.rand();
            dist1_clip = noise.randint(1,DISTORTION_LAST);
            dist2_clip = noise.randint(1,DISTORTION_LAST);
            dist3_clip = noise.randint(1,DISTORTION_LAST);
            dist4_clip = noise.randint(1,DISTORTION_LAST);
            printf("%d %d\n",dist1_clip,dist2_clip);
            distortion_morph = noise.rand();
        }
        virtual DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = -1, DspFloatType Y = 1, DspFloatType B=0)
        {
            
            DspFloatType K = G;
            DspFloatType bt = bias;
            bias = B;
            I += bias;
            I *= preGain;

            DspFloatType d1 = distortionClip(dist1_clip,I,K);
            DspFloatType d2 = distortionClip(dist2_clip,I,K);
            DspFloatType d3 = distortionClip(dist3_clip,I,K);
            DspFloatType d4 = distortionClip(dist4_clip,I,K);
            
            DspFloatType distortion1;
            if(X >= 0) distortion1 = d1;
            else distortion1 = d2;

            DspFloatType distortion2;
            if(Y >= 0) distortion2 = d3;
            else distortion2 = d4;

            DspFloatType distortion = function_morpher(distortion1,distortion2,distortion_morph);

            DspFloatType r = clamp(A*distortion,min,max);
            bias = bt;
            r -= bias;
            return tanh(postGain*r);

        }
    };


    struct DiodeClipperAmplifier : public Amplifier
    {
        DspFloatType bias = 0.0;
        DspFloatType preG = 1.0;
        DspFloatType postG = 1.0;
        DspFloatType cmin  = -1.0;
        DspFloatType cmax  = 1.0;

        DiodeClipperAmplifier()
        {

        }

        virtual DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = -1, DspFloatType Y = 1, DspFloatType B=0)
        {
            return 0.0;            
        }
    };

    struct DiodeSimAmplifier : public Amplifier
    {

    };

    struct DiodAmplifier : public Amplifier
    {

    };

    struct WDFDiodeClipperAmplifier : public Amplifier
    {

    };

    struct ValveAmplifier : public Amplifier
    {

    };


    struct Amplifier8 : public Amplifier
    {
        Amplifier amps[4];

        Amplifier8() : Amplifier() {

        }

        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
        {
            DspFloatType r = I;
            for(size_t i = 0; i < 4; i++)
                r = amps[i].Tick(r);
            return r;
        }
        DspFloatType Serial(DspFloatType I, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
        {
            DspFloatType r = 1;
            for(size_t i = 0; i < 4; i++)
                r *= amps[i].Tick(I);
            return r;
        }
        DspFloatType Parallel(DspFloatType I, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
        {
            DspFloatType r = 0;
            for(size_t i = 0; i < 4; i++)
                r += amps[i].Tick(I);
            return r/4.0f;
        }
        DspFloatType Modulus(DspFloatType I, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
        {
            DspFloatType r = I;
            for(size_t i = 0; i < 4; i++)
                r = fmod(amps[i].Tick(r),r);
            return r;
        }
        void RandomClip() {
            for(size_t i = 0; i < 4; i++)
                amps[i].RandomClip();
        }
    };


    template<int N>
    struct AmplifierN : public Amplifier
    {
        Amplifier amps[N];

        AmplifierN() : Amplifier() {

        }

        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
        {
            DspFloatType r = I;
            for(size_t i = 0; i < N; i++)
                r = amps[i].Tick(r);
            return r;
        }
        DspFloatType Serial(DspFloatType I, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
        {
            DspFloatType r = 1;
            for(size_t i = 0; i < N; i++)
                r *= amps[i].Tick(I);
            return r;
        }
        DspFloatType Parallel(DspFloatType I, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
        {
            DspFloatType r = 0;
            for(size_t i = 0; i < N; i++)
                r += amps[i].Tick(I);
            return r/(DspFloatType)N;
        }
        DspFloatType Modulus(DspFloatType I, DspFloatType A=1, DspFloatType X = -1, DspFloatType Y=1)
        {
            DspFloatType r = I;
            for(size_t i = 0; i < N; i++)
                r = fmod(amps[i].Tick(r),r);
            return r;
        }
        void RandomClip() {
            for(size_t i = 0; i < N; i++)
                amps[i].RandomClip();
        }
    };
}