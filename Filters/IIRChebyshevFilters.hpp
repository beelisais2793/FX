#pragma once

// Homebrew chebyshev filters

namespace Filters::IIR::ChebyshevFilters
{

    std::complex<DspFloatType> ChebyshevH0(DspFloatType N, DspFloatType r=1.0)
    {        
        DspFloatType e     = sqrt(pow(10.0,r/10.0)-1.0);
        DspFloatType theta = (M_PI/2.0)+((2*1-1.0)/(2.0*N))*M_PI;
        DspFloatType phi   = (1.0/N)*asinh(1.0/e);
        std::complex<DspFloatType> P(sinh(phi)*cos(theta),-cosh(phi)*sin(theta));        
        for(size_t K=2; K <= N; K++)
        {
            e     = sqrt(pow(10.0,r/10.0)-1.0);
            theta = (M_PI/2.0)+((2*K-1.0)/(2.0*N))*M_PI;
            phi   = (1.0/N)*asinh(1.0/e);
            std::complex<DspFloatType> p(sinh(phi)*cos(theta),-cosh(phi)*sin(theta));        
            P *= -p;        
        }
        if(fmod(N,2) == 0) return P/sqrt(1 + e*e);
        return P;
    }

    // Chebyshev2 = 1/pole
    std::complex<DspFloatType> ChebyshevPole(DspFloatType K, DspFloatType N, DspFloatType r=1.0)
    {      
        DspFloatType e     = sqrt(pow(10.0,r/10.0)-1.0);
        DspFloatType theta = (M_PI/2.0)+((2*K-1.0)/(2.0*N))*M_PI;
        DspFloatType phi   = (1.0/N)*asinh(1.0/e);
        std::complex<DspFloatType> p(sinh(phi)*cos(theta),-cosh(phi)*sin(theta));
        return p;
    }

    std::vector<std::complex<DspFloatType>> ChebyshevPoles(DspFloatType N, DspFloatType r)
    {
        std::vector<std::complex<DspFloatType>> out;
        for(size_t K = 1; K <= N; K++)
        {
            out.push_back(ChebyshevPole(K,N,r));
        }
        return out;
    }

    BiquadSOS ChebyshevILowPassSolver(int n, DspFloatType rippledb=1.0)
    {
        BiquadSOS sos;
        if (n % 2 != 0)
            sos.resize(n / 2 + 1);
        else
            sos.resize(n / 2);

        int total = n / 2;
        int index = 0;
        DspFloatType H0 = abs(ChebyshevH0(n,rippledb));
        std::vector<std::complex<DspFloatType>> poles = ChebyshevPoles(n,rippledb);
        int polepos=0;    
        if (n % 2 != 0)
        {
            sos[0].z[0] = H0;
            sos[0].z[1] = 0;
            sos[0].z[2] = 0;
            sos[0].p[0] = 1;
            sos[0].p[1] = abs(poles[0]);
            sos[0].p[2] = 0;            
            index = 1;
            polepos++;
        }
        
        for (int k = 0; k < total; k++)
        {            
            // DspFloatType X  = -2*(R/Q)*std::cos(M_PI*X1/X2);
            // -p1s -p2s = -s(p1+p2)
            // (s-p1)(s-p2) = s^2 -s*(p1+p2) + p1*p2
            DspFloatType X = abs((poles[polepos] + poles[polepos+1]));
            DspFloatType Y = abs(poles[polepos] * poles[polepos+1]);
            
            polepos += 2;
            sos[index].z[0] = H0/Y;
            sos[index].z[1] = 0;
            sos[index].z[2] = 0;
            sos[index].p[0] = 1;
            sos[index].p[1] = X/Y;
            sos[index].p[2] = 1/Y;
            index++;
        }
        return sos;
    }
    
    struct ChebyshevILowPassFilter12db
    {

        BiquadTypeI bc;
        DspFloatType Fc, Sr, Q;
        

        ChebyshevILowPassFilter12db()
        {            
            setCutoff(1000.0f);
        }
        void setQ(DspFloatType q)
        {
            Q = q;
        }        
        void setCutoff(float fc)
        {
            Fc = fc;

            if (Fc == 0)
                return;

            BiquadSOS s           = ChebyshevILowPassSolver(2);
            BiquadSection section = Filters::IIR::AnalogBiquadSection(s[0], fc, sampleRate);

            bc.setCoefficients(section);
        }
        enum {
            PORT_CUTOFF,
            PORT_Q,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: setCutoff(v); break;
                case PORT_Q: setQ(v); break;
            }
        }
        DspFloatType Tick(DspFloatType I)
        {
            DspFloatType y = bc.Tick(I);
            return y;
        }
    };

    
    // Chebyshev2 = 1/pole
    std::complex<DspFloatType> Chebyshev2Zero(DspFloatType K, DspFloatType N, DspFloatType r=1.0)
    {      
        DspFloatType uk    = ((2*K-1)/N)*(M_PI/2.0);
        std::complex<DspFloatType> p(0,cos(uk));
        return DspFloatType(1.0)/p;
    }

    // Chebyshev2 = 1/pole
    std::complex<DspFloatType> Chebyshev2Pole(DspFloatType K, DspFloatType N, DspFloatType r=1.0)
    {      
        DspFloatType e     = 1.0/sqrt(pow(10.0,r/10.0)-1.0);
        DspFloatType theta = (M_PI/2.0)+((2*K-1.0)/(2.0*N))*M_PI;
        DspFloatType phi   = (1.0/N)*asinh(1.0/e);
        std::complex<DspFloatType> p(sinh(phi)*cos(theta),-cosh(phi)*sin(theta));
        return DspFloatType(1.0)/p;
    }

     std::vector<std::complex<DspFloatType>> Chebyshev2Zeros(DspFloatType N, DspFloatType r)
    {
        std::vector<std::complex<DspFloatType>> out;
        for(size_t K = 1; K <= N; K++)
        {
            out.push_back(Chebyshev2Zero(K,N,r));
        }
        return out;
    }
     std::vector<std::complex<DspFloatType>> Chebyshev2Poles(DspFloatType N, DspFloatType r)
    {
        std::vector<std::complex<DspFloatType>> out;
        for(size_t K = 1; K <= N; K++)
        {
            out.push_back(Chebyshev2Pole(K,N,r));
        }
        return out;
    }

    BiquadSOS ChebyshevIILowPassSolver(int n, DspFloatType rippledb=1.0)
    {
        BiquadSOS sos;
        if (n % 2 != 0)
            sos.resize(n / 2 + 1);
        else
            sos.resize(n / 2);

        int total = n / 2;
        int index = 0;        
        std::vector<std::complex<DspFloatType>> zeros = Chebyshev2Zeros(n,rippledb);
        std::vector<std::complex<DspFloatType>> poles = Chebyshev2Poles(n,rippledb);
        int polepos=0;    
        if (n % 2 != 0)
        {
            sos[0].z[0] = abs(zeros[0]);
            sos[0].z[1] = 0;
            sos[0].z[2] = 0;
            sos[0].p[0] = 1;
            sos[0].p[1] = abs(poles[0]);
            sos[0].p[2] = 0;            
            index = 1;
            polepos++;
        }
        
        for (int k = 0; k < total; k++)
        {            
            // DspFloatType X  = -2*(R/Q)*std::cos(M_PI*X1/X2);
            // -p1s -p2s = -s(p1+p2)
            // (s-z1)(s-z2) = s^2 -s*(z1+z2) +z1*z2
            // (s-p1)(s-p2) = s^2 -s*(p1+p2) + p1*p2
            DspFloatType ZX = abs((zeros[polepos] + zeros[polepos+1])); 
            DspFloatType ZY = abs(zeros[polepos] * zeros[polepos+1]);
            DspFloatType PX = abs((poles[polepos] + poles[polepos+1]));
            DspFloatType PY = abs(poles[polepos] * poles[polepos+1]);            
            polepos += 2;
            sos[index].z[0] = ZY/PY;
            sos[index].z[1] = 0;
            sos[index].z[2] = ZX/PY;
            sos[index].p[0] = 1;
            sos[index].p[1] = PX/PY;
            sos[index].p[2] = 1/PY;
            index++;
        }
        return sos;
    }
    
    struct ChebyshevIILowPassFilter12db
    {

        BiquadTypeI bc;
        DspFloatType Fc, Sr, Q;
        

        ChebyshevIILowPassFilter12db()
        {            
            setCutoff(1000.0f);
        }
        void setQ(DspFloatType q)
        {
            Q = q;
        }        
        void setCutoff(float fc)
        {
            Fc = fc;

            if (Fc == 0)
                return;

            BiquadSOS s           = ChebyshevIILowPassSolver(2);
            BiquadSection section = Filters::IIR::AnalogBiquadSection(s[0], fc, sampleRate);

            bc.setCoefficients(section);
        }
        enum {
            PORT_CUTOFF,
            PORT_Q,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: setCutoff(v); break;
                case PORT_Q: setQ(v); break;
            }
        }
        DspFloatType Tick(DspFloatType I)
        {
            DspFloatType y = bc.Tick(I);
            return y;
        }
    };

}