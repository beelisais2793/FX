////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Filters MusicDSP Biquad 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cfloat>
#define SPN FLT_MIN

namespace SoundAlchemy::Biquads
{
    template<typename T>
    struct TBiquadFilter : public Object<T>
    {
        T last,y;
        T a0,a1,a2,b1,b2;
        T x1,x2,y1,y2;
        T cx,cx1,cx2,cy1,cy2;
        T SR;

    // presence
    // shelve
    // shelve lowpass
    // bandpass
    // peaknotch
    // peaknotch2
    // highpass = change lowpass to highpass
    // State Variable Lowpass 

        TBiquadFilter(T fs = 44100)    
        {
            x1 = 0.0;
            x2 = 0.0;
            y1 = 0.0;
            y2 = 0.0;        
            x2=x2=y1=y2=0.0;
            cx=cx1=cx2=cy1=cy2=0;
            SR = fs;
        }

        T bw2angle(T a,T bw)    
        {
            T t,d,sn,cs,mag,delta,theta,tmp,a2,a4,asnd;

            t = std::tan(2.0*M_PI*bw);
            a2 = a*a;
            a4 = a2*a2;
            d = 2.0*a2*t;
            sn = (1.0 + a4)*t;
            cs = (1.0 - a4);
            mag = std::sqrt(sn*sn + cs*cs);
            d /= mag;
            delta = std::atan2(sn,cs);
            asnd = std::asin(d);
            theta = 0.5*(M_PI - asnd - delta);
            tmp = 0.5*(asnd-delta);
            if ((tmp > 0.0) && (tmp < theta)) theta = tmp;
            return(theta/(2.0*M_PI));
        }

        void presence(T cf,T boost,T bw)    
        {
            T a,A,F,xfmbw,C,tmp,alphan,alphad,b0,recipb0,asq,F2,a2plus1,ma2plus1;

            a = std::tan(M_PI*(cf-0.25));
            asq = a*a;
            A = std::pow(10.0,boost/20.0);
            if ((boost < 6.0) && (boost > -6.0)) F = sqrt(A);
            else if (A > 1.0) F = A/std::sqrt(2.0);
            else F = A*std::sqrt(2.0);
            xfmbw = bw2angle(a,bw);

            C = 1.0/std::tan(2.0*M_PI*xfmbw);
            F2 = F*F;
            tmp = A*A - F2;
            if (fabs(tmp) <= SPN) alphad = C;
            else alphad = std::sqrt(C*C*(F2-1.0)/tmp);
            alphan = A*alphad;

            a2plus1 = 1.0 + asq;
            ma2plus1 = 1.0 - asq;
            a0 = a2plus1 + alphan*ma2plus1;
            a1 = 4.0*a;
            a2 = a2plus1 - alphan*ma2plus1;

            b0 = a2plus1 + alphad*ma2plus1;
            b2 = a2plus1 - alphad*ma2plus1;

            recipb0 = 1.0/b0;
            a0 *= recipb0;
            a1 *= recipb0;
            a2 *= recipb0;
            b1 = *a1;
            b2 *= recipb0;
        }

        void shelve(T cf,T boost)    
        {
            T a,A,F,tmp,b0,recipb0,asq,F2,gamma2,siggam2,gam2p1;
            T gamman,gammad,ta0,ta1,ta2,tb0,tb1,tb2,aa1,ab1;

            a = std::tan(M_PI*(cf-0.25));
            asq = a*a;
            A = std::pow(10.0,boost/20.0);
            if ((boost < 6.0) && (boost > -6.0)) F = std::sqrt(A);
            else if (A > 1.0) F = A/std::sqrt(2.0);
            else F = A*std::sqrt(2.0);

            F2 = F*F;
            tmp = A*A - F2;
            if (fabs(tmp) <= SPN) gammad = 1.0;
            else gammad = std::pow((F2-1.0)/tmp,0.25);
            gamman = std::sqrt(A)*gammad;

            gamma2 = gamman*gamman;
            gam2p1 = 1.0 + gamma2;
            siggam2 = 2.0*std::sqrt(2.0)/2.0*gamman;
            ta0 = gam2p1 + siggam2;
            ta1 = -2.0*(1.0 - gamma2);
            ta2 = gam2p1 - siggam2;

            gamma2 = gammad*gammad;
            gam2p1 = 1.0 + gamma2;
            siggam2 = 2.0*std::sqrt(2.0)/2.0*gammad;
            tb0 = gam2p1 + siggam2;
            tb1 = -2.0*(1.0 - gamma2);
            tb2 = gam2p1 - siggam2;

            aa1 = a*ta1;
            a0 = ta0 + aa1 + asq*ta2;
            a1 = 2.0*a*(ta0+ta2)+(1.0+asq)*ta1;
            a2 = asq*ta0 + aa1 + ta2;

            ab1 = a*tb1;
            b0 = tb0 + ab1 + asq*tb2;
            b1 = 2.0*a*(tb0+tb2)+(1.0+asq)*tb1;
            b2 = asq*tb0 + ab1 + tb2;

            recipb0 = 1.0/b0;
            a0 *= recipb0;
            a1 *= recipb0;
            a2 *= recipb0;
            b1 *= recipb0;
            b2 *= recipb0;
        }

        
        void setfilter_presence(T freq,T boost,T bw)        
            {
            presence(freq/(T)SR,boost,bw/(T)SR);
            cy1 = cy1;
            cy2 = cy2;
        }

        void setfilter_shelve(T freq, T boost)        
        {
            shelve(freq/(T)SR,boost);
            cy1 = cy1;
            y2 =  cy2;
        }
        
        void setfilter_shelvelowpass(T freq, T boost)    
        {
            T gain;

            gain = std::pow(10.0,boost/20.0);
            shelve(freq/(T)SR,boost);
            cx /= gain; 
            cx1 /= gain; 
            cx2 /= gain; 
            cy1 = -cy1;
            cy2 = -cy2;
        }

        
        /*
        * As in ''An introduction to digital filter theory'' by Julius O. Smith
        * and in Moore's book; I use the normalized version in Moore's book.
        */
        void setfilter_2polebp(T freq, T R)
        {
            T theta;

            theta = 2.0*M_PI*freq/(T)SR;
            cx = 1.0-R;
            cx1 = 0.0;
            cx2 = -(1.0-R)*R;
            cy1 = 2.0*R*std::cos(theta);
            cy2 = -R*R;
        }

        
        /*
        * As in
        *   Stanley A. White
        *   Design of a digital biquadratic peaking or notch filter
        *   for digital audio equalization
        *   JAES, Vol. 34, No. 6, 1986 June
        */
        void setfilter_peaknotch(T freq,T M,T bw)    
        {
            T w0,p,om,ta,d;

            w0 = 2.0*M_PI*freq;
            assert(!(1.0/std::sqrt(2.0) < M) && (M < std::sqrt(2.0)));            
            if (M <= 1.0/std::sqrt(2.0)) p = std::sqrt(1.0-2.0*M*M);
            if (std::sqrt(2.0) <= M) p = std::sqrt(M*M-2.0);
            om = 2.0*M_PI*bw;
            ta = std::tan(om/((T)SR*2.0));
            d = p+ta;
            cx = (p+M*ta)/d;
            cx1 = -2.0*p*std::cos(w0/(T)SR)/d;
            cx2 = (p-M*ta)/d;
            cy1 = 2.0*p*std::cos(w0/(T)SR)/d;
            cy2 = -(p-ta)/d;
        }

        
        /*
        * Some JAES's article on ladder filter.
        * freq (Hz), gdb (dB), bw (Hz)
        */
        void setfilter_peaknotch2(T freq,T gdb,T bw)
        {
            T k,w,bwr,abw,gain;

            k = std::pow(10.0,gdb/20.0);
            w = 2.0*M_PI*freq/(T)SR;
            bwr = 2.0*M_PI*bw/(T)SR;
            abw = (1.0-std::tan(bwr/2.0))/(1.0+std::tan(bwr/2.0));
            gain = 0.5*(1.0+k+abw-k*abw);
            cx = 1.0*gain;
            cx1 = gain*(-2.0*std::cos(w)*(1.0+abw))/(1.0+k+abw-k*abw);
            cx2 = gain*(abw+k*abw+1.0-k)/(abw-k*abw+1.0+k);
            cy1 = 2.0*cos(w)/(1.0+std::tan(bwr/2.0));
            cy2 = -abw;
        }

        T Tick(T x, T A = 1, T F = 1, T P = 1)    
        {
            last = x;
            y = cx * A*(input_gain*x) + cx1 * x1 + cx2 * x2;
            + cy1 * y1 + cy2 * y2;
            x2 = x1;
            x1 = x;
            y2 = y1;
            y1 = y;            
            return clamp(output_gain*y,minA,maxA);
        }
    };
}

#undef SPN