#pragma once

struct ThirdOrderAllPole
{
    DspFloatType b0,b1,b2,a1,a2,a3,a4,a5;
    DspFloatType fc,sr,R;
    DspFloatType y,x0,y1,y2,y3,x1,x2;

    //(1-exp1)(1-2RCOS1-R*R2) = 1 - 2RCOS1 - R*R2 + exp1 + 2RCOSEXP2 -expR*R3
    ThirdOrderAllPole()
    {
        fc = 440.0f;
        sr = sampleRate;        
        R  = 0.9;
        y = x0 = y1 = y2 = y3 = x1 = x2 = 0;
        setCutoff(fc);
    }
    void setRadius(DspFloatType r) {
        if(r < 0 || r >= 1.0) return;
        R = r;
    }
    void setCutoff(DspFloatType f)
    {        
        if(f < 0 || f > sr/2) return;
        fc = f;
        DspFloatType theta = exp(-2*M_PI*fc/sr);
        
        
        a1 = -theta;
        a2 = a1*a1;
        a3 = a1*a2;
        
        b0 = pow((1-theta),3);
        
        
    }
    DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
    {
        DspFloatType f = fc;
        DspFloatType r = R;
        //R *= fabs(Y);
        //setCutoff(f*fabs(X));
        x0 = I;
        y  = b0*I - a1*y1 - a2*y2 - a3*y3;
        
        x2 = x1;
        x1 = x0;
        y3 = y2;
        y2 = y1;
        y1 = y;
        //R = r;
        //setCutoff(f);
        return A*y;
    }
};

