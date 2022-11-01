#pragma once

namespace Analog::Filters::VecSVF
{
    template<typename SIMD>
    struct VecSVF
    {    
        float fc,fs,q,K;
        SIMD lp,hp,bp,ubp,shelf,notch,apf,peak;
        SIMD z1,z2;
        

        VecSVF(double Fs, double Fc, double Q) {
            fc = Fc;
            fs = Fs;
            q = Q + 0.5;        
            z1 = z2 = 0.0;
            K = 1;        
        }
        void setCutoff(float f) {
            if(f < 0) f = 0;    
            if(f > fs/2) f = fs/2;            
            fc = 0.5*f;        
        }
        void setQ(float Q) {           
            q = Q+0.5;        
        }

        SIMD Tick(SIMD I, double A = 1, double X = 1, double Y = 1, double Z = 1, double OA=1, double minC = -1, double maxC = 1)
        {                
            float wd = 2*M_PI*fc;
            float T  = 1/fs;
            float temp = X*wd*T/2;
            float wa;

            if(fabs(temp) != M_PI/2)
                wa = (2/T)*tan(temp);
            else
                wa = (2/T)*tan(temp*0.995);
            
            float g  = wa*T/2;
            float R  = 1.0/(2*(q));
            
            SIMD xn = A*I;
            //if(xn < minC) xn = minC;
            //if(xn > maxC) xn = maxC;
            //xn = xn;
            
            hp = (xn - (2*R+g)*z1 - z2) / (1.0 + 2*R*g + g*g) - R*z2;
            //hp = tanh(hp);
            bp = g*hp + z1;        
            lp = g*bp + z2;
            
            // not sure these work right yet
            ubp = 2 * R * bp;
            // dont know exactly what K is it's not explained
            shelf = xn + 2*K*R*bp;
            notch = xn - 2*R*bp;
            apf   = xn - 4*R*bp;
            peak  = lp - hp;

            // delay feedback
            z1 = (g*hp + bp);
            z2 = (g*bp + lp);
                    
            return lp;
        }    
    };
}
