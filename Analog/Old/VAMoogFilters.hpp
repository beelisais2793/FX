
#pragma once 

#include <cmath>

namespace Analog::Filters::MoogFilters
{


    template<typename T>
    struct TMoogFilter : public FilterProcessor
    {    
        TMoogFilter();
        ~TMoogFilter();

        void init();
        void calc();
        T process(T x);
                
        T getCutoff();
        void setCutoff(T c);
        T getRes();
        void setRes(T r);
    
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*process(I);
        }

        T cutoff;
        T res;
        T fs;
        T y1,y2,y3,y4;
        T oldx;
        T oldy1,oldy2,oldy3;
        T x;
        T r;
        T p;
        T k;
    };

    template<typename T>
    TMoogFilter<T>::TMoogFilter() : FilterProcessor()
    {
        fs=44100.0;
        init();
    }

    template<typename T>
    TMoogFilter<T>::~TMoogFilter()
    {
    }

    template<typename T>
    void TMoogFilter<T>::init()
    {
        // initialize values
        y1=y2=y3=y4=oldx=oldy1=oldy2=oldy3=0;
        calc();
    };

    template<typename T>
    void TMoogFilter<T>::calc()
    {
        T f = (cutoff+cutoff) / fs; //[0 - 1]
        p=f*(1.8f-0.8f*f);
        k=p+p-1.f;

        T t=(1.f-p)*1.386249f;
        T t2=12.f+t*t;
        r = res*(t2+6.f*t)/(t2-6.f*t);
    };

    template<typename T>
    T TMoogFilter<T>::process(T input)
    {
        // process input
        x = bpsigmoid(input) - r*y4;

        //T q = p;
        //p = p+p;
        //Four cascaded onepole filters (bilinear transform)
        y1= x*p +  oldx*p - k*y1;    
        y2=y1*p + oldy1*p - k*y2;    
        y3=y2*p + oldy2*p - k*y3;    
        y4=y3*p + oldy3*p - k*y4;
        //p = q;
        //Clipper band limited sigmoid
        y4-=((y4*y4*y4)/6.f);

        oldx = x; oldy1 = y1; oldy2 = y2; oldy3 = y3;
        return y4;
    }

    template<typename T>
    T TMoogFilter<T>::getCutoff(){ return cutoff; }

    template<typename T>
    void TMoogFilter<T>::setCutoff(T c){ cutoff=c; calc(); }

    template<typename T>
    T TMoogFilter<T>::getRes(){ return res; }

    template<typename T>
    void TMoogFilter<T>::setRes(T r) { res=r; calc(); }
}