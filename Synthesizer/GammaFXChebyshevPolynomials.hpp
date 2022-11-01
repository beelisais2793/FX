#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"


namespace Gamma::FX
{

    template<int N>
    struct ChebyN : public FunctionProcessorPlugin<gam::ChebyN<N,double>>
    {
        ChebyN() : FunctionProcessorPlugin<gam::ChebyN<N,double>>()
        {

        }
        enum {
            PORT_ZERO,
            PORT_WET,
            PORT_SET,
        };
        double H[N];
        bool wet = false;
        void setPort(int port, double v) {
            switch(port) {
                case PORT_ZERO: this->zero(); break;                
                case PORT_WET: this->wet = !this->wet; break;
                case PORT_SET: this->set(H); break;
            }
        }
        void setPort2(int port, double x, double y) {
            if(port == PORT_SET)
            {
                H[(int)x] = y;             
            }            
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            if(wet) return A*this->wet(I);
            return A*(*this)(I);
        }
    };
    using Cheby2 = ChebyN<2>;
    using Cheby3 = ChebyN<3>;
    using Cheby4 = ChebyN<4>;
    using Cheby5 = ChebyN<5>;
    using Cheby6 = ChebyN<6>;
    using Cheby7 = ChebyN<7>;
    using Cheby8 = ChebyN<8>;
    using Cheby9 = ChebyN<9>;
    using Cheby10 = ChebyN<10>;
    using Cheby11 = ChebyN<11>;
    using Cheby12 = ChebyN<12>;
    using Cheby13 = ChebyN<13>;
    using Cheby14 = ChebyN<14>;
    using Cheby15 = ChebyN<15>;
    using Cheby16 = ChebyN<16>;
    using Cheby17 = ChebyN<17>;
    using Cheby18 = ChebyN<18>;
    using Cheby19 = ChebyN<19>;
    using Cheby20 = ChebyN<20>;    
}