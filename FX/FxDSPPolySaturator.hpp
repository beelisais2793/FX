#pragma once

namespace DSPFX
{

    struct FxPolySaturator
    {
        PolySaturator * sat;
        double N;

        FxPolySatuator(DspFloatType n) {
            N = n;
            sat = PolySaturatorInitD(n);
            assert(sat != NULL);
        }
        ~FxPolySaturator() {
            if(sat) PolySaturatorFreeD(sat);
        }
        void setN(DspFloatType n) {
            N = n;
            PolySaturatorSetND(sat,n);
        }
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            PolySaturatorProcessD(sat,out,in,n);
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            double t = N;
            setN(N*X*Y);
            double r = PolySaturatorTickD(I);
            setN(t);
            return A*r;
        }
        enum {
            PORT_N,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_N: setN(v); break;
                default: printf("no port %d\n",port);
            }
            
        }        
    };
}
