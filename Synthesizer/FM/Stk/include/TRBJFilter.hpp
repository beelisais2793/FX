
#pragma once
#include "SoundAlchemy.hpp"

namespace SoundAlchemy::Filters
{
    template<typename T>
    class TRBJFilter
    {
    public:
        
        TRBJFilter()
        {
            // reset filter coeffs
            b0a0=b1a0=b2a0=a1a0=a2a0=0.0;
            
            // reset in/out history
            ou1=ou2=in1=in2=0.0f;	
        };

        T filter(T in0)
        {
            // filter
            T const yn = b0a0*in0 + b1a0*in1 + b2a0*in2 - a1a0*ou1 - a2a0*ou2;

            // push in/out buffers
            in2=in1;
            in1=in0;
            ou2=ou1;
            ou1=yn;

            // return output
            return yn;
        };

        void calc_filter_coeffs(int const type,double const frequency,double const sample_rate,double const q,double const db_gain,bool q_is_bandwidth)
        {
            // temp pi
            double const temp_pi=3.1415926535897932384626433832795;
            
            // temp coef vars
            double alpha,a0,a1,a2,b0,b1,b2;

            // peaking, lowshelf and hishelf
            if(type>=6)
            {
                double const A		=	std::pow(10.0,(db_gain/40.0));
                double const omega	=	2.0*temp_pi*frequency/sample_rate;
                double const tsin	=	std::sin(omega);
                double const tcos	=	std::cos(omega);
                
                if(q_is_bandwidth)
                alpha=tsin*std::sinh(std::log(2.0)/2.0*q*omega/tsin);
                else
                alpha=tsin/(2.0*q);

                double const beta	=	std::sqrt(A)/q;
                
                // peaking
                if(type==6)
                {
                    b0=T(1.0+alpha*A);
                    b1=T(-2.0*tcos);
                    b2=T(1.0-alpha*A);
                    a0=T(1.0+alpha/A);
                    a1=T(-2.0*tcos);
                    a2=T(1.0-alpha/A);
                }
                
                // lowshelf
                if(type==7)
                {
                    b0=T(A*((A+1.0)-(A-1.0)*tcos+beta*tsin));
                    b1=T(2.0*A*((A-1.0)-(A+1.0)*tcos));
                    b2=T(A*((A+1.0)-(A-1.0)*tcos-beta*tsin));
                    a0=T((A+1.0)+(A-1.0)*tcos+beta*tsin);
                    a1=T(-2.0*((A-1.0)+(A+1.0)*tcos));
                    a2=T((A+1.0)+(A-1.0)*tcos-beta*tsin);
                }

                // hishelf
                if(type==8)
                {
                    b0=T(A*((A+1.0)+(A-1.0)*tcos+beta*tsin));
                    b1=T(-2.0*A*((A-1.0)+(A+1.0)*tcos));
                    b2=T(A*((A+1.0)+(A-1.0)*tcos-beta*tsin));
                    a0=T((A+1.0)-(A-1.0)*tcos+beta*tsin);
                    a1=T(2.0*((A-1.0)-(A+1.0)*tcos));
                    a2=T((A+1.0)-(A-1.0)*tcos-beta*tsin);
                }
            }
            else
            {
                // other filters
                double const omega	=	2.0*temp_pi*frequency/sample_rate;
                double const tsin	=	std::sin(omega);
                double const tcos	=	std::cos(omega);

                if(q_is_bandwidth)
                alpha=tsin*std::sinh(std::log(2.0)/2.0*q*omega/tsin);
                else
                alpha=tsin/(2.0*q);

                
                // lowpass
                if(type==0)
                {
                    b0=(1.0-tcos)/2.0;
                    b1=1.0-tcos;
                    b2=(1.0-tcos)/2.0;
                    a0=1.0+alpha;
                    a1=-2.0*tcos;
                    a2=1.0-alpha;
                }

                // hipass
                if(type==1)
                {
                    b0=(1.0+tcos)/2.0;
                    b1=-(1.0+tcos);
                    b2=(1.0+tcos)/2.0;
                    a0=1.0+ alpha;
                    a1=-2.0*tcos;
                    a2=1.0-alpha;
                }

                // bandpass csg
                if(type==2)
                {
                    b0=tsin/2.0;
                    b1=0.0;
                    b2=-tsin/2;
                    a0=1.0+alpha;
                    a1=-2.0*tcos;
                    a2=1.0-alpha;
                }

                // bandpass czpg
                if(type==3)
                {
                    b0=alpha;
                    b1=0.0;
                    b2=-alpha;
                    a0=1.0+alpha;
                    a1=-2.0*tcos;
                    a2=1.0-alpha;
                }

                // notch
                if(type==4)
                {
                    b0=1.0;
                    b1=-2.0*tcos;
                    b2=1.0;
                    a0=1.0+alpha;
                    a1=-2.0*tcos;
                    a2=1.0-alpha;
                }

                // allpass
                if(type==5)
                {
                    b0=1.0-alpha;
                    b1=-2.0*tcos;
                    b2=1.0+alpha;
                    a0=1.0+alpha;
                    a1=-2.0*tcos;
                    a2=1.0-alpha;
                }
            }

            // set filter coeffs
            b0a0=T(b0/a0);
            b1a0=T(b1/a0);
            b2a0=T(b2/a0);
            a1a0=T(a1/a0);
            a2a0=T(a2/a0);
        };

    private:

        // filter coeffs
        T b0a0,b1a0,b2a0,a1a0,a2a0;

        // in/out history
        T ou1,ou2,in1,in2;
    };
}    