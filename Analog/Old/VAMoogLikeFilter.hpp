#pragma once 

#include <cmath>
#include "Undenormal.hpp"

namespace Analog::Filters::MoogLike
{
    struct MoogLike : public FilterProcessor
    {
        
        enum {
            LOWPASS,
            HIGHPASS
        };
        int type = LOWPASS;
        double coef[9];
        double d[4];
        double omega; //peak freq        
        double  fs,res;
        double  in,out;

        // calculating coefficients:

        double k,p,q,g,a;
        double a0,a1,a2,a3,a4;
        
        MoogLike(double Fc, double Q, double G, double Fs) : FilterProcessor()
        {
            omega = Fc;
            q  = Q;
            fs = Fs;
            g  = G;
            k =p=q=a=a0=a1=a2=a3=a4=0;
        }

        void SetCoefficients(double Fc, double R)
        {
            omega = Fc;
            q     = R;
            k=(4.0*g-3.0)/(g+1.0);
            p=1.0-0.25*k;
            p*=p;
            
            if(type == LOWPASS) {
                // LP:
                a=1.0/(std::tan(0.5*omega)*(1.0+p));
                p=1.0+a;
                q=1.0-a;

                a0= 1.0/(k+p*p*p*p);
                a1= 4.0*(k+p*p*p*q);
                a2= 6.0*(k+p*p*q*q);
                a3= 4.0*(k+p*q*q*q);
                a4= (k+q*q*q*q);
                p = a0*(k+1.0);

                coef[0]=p;
                coef[1]=4.0*p;
                coef[2]=6.0*p;
                coef[3]=4.0*p;
                coef[4]=p;
                coef[5]=-a1*a0;
                coef[6]=-a2*a0;
                coef[7]=-a3*a0;
                coef[8]=-a4*a0;
            }
            else {
                // or HP:
                a=std::tan(0.5*omega)/(1.0+p);
                p=a+1.0;
                q=a-1.0;

                a0=1.0/(p*p*p*p+k);
                a1=4.0*(p*p*p*q-k);
                a2=6.0*(p*p*q*q+k);
                a3=4.0*(p*q*q*q-k);
                a4=    (q*q*q*q+k);
                p=a0*(k+1.0);

                coef[0]=p;
                coef[1]=-4.0*p;
                coef[2]=6.0*p;
                coef[3]=-4.0*p;
                coef[4]=p;
                coef[5]=-a1*a0;
                coef[6]=-a2*a0;
                coef[7]=-a3*a0;
                coef[8]=-a4*a0;
            }
        }
        double Tick(double I, double A = 1, double X=0, double Y=0)
        {
            Undenormal denormal;
                    
            in = I;
            // per sample:
            out=coef[0]*in+d[0];
            d[0]=coef[1]*in+coef[5]*out+d[1];
            d[1]=coef[2]*in+coef[6]*out+d[2];
            d[2]=coef[3]*in+coef[7]*out+d[3];
            d[3]=coef[4]*in+coef[8]*out;
            return out;
        }
    };
}