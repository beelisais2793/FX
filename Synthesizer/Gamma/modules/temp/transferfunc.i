%{
#include "Gamma/TransferFunc.h"
%}

namespace gam{

    class TransferFunc {
    public:
        typedef std::complex<double> Complex;
        struct DelayUnit;

    
        TransferFunc(double gain=1);
        TransferFunc& resize(int nx, int ny);

        DelayUnit& x(int idx);
        DelayUnit& y(int idx);
        
        TransferFunc& addX(double c, double d);
        TransferFunc& addY(double c, double d);
        TransferFunc& clear();
        TransferFunc& gain(double v);
        
        %extend 
        {
            Complex Tick(double f) { return (*$self)(f); }
            Complex TickZ(Complex z) { return (*$self)(z); }
        }
    };

    struct TransferFunc::DelayUnit{
		/// param[in] c		weighting coefficient
		/// param[in] d		delay in samples
		DelayUnit(double c_=0, double d_=0): c(c_), d(d_){}

		DelayUnit& weight(double v){ c=v; return *this; }

		DelayUnit& delay(double v){ d=v; return *this; }

		// H(z) = c z^d
		Complex response(Complex z){
			return c * pow(z,-d);
		}
		
		/// \param[in] f	frequency, in radians
		Complex response(double f){
			double phs = f*d;
			return Complex(c*cos(phs), c*sin(phs));
			// return response(Complex(cos(f), sin(f)));
		}

		double c, d;
	};


} // gam::