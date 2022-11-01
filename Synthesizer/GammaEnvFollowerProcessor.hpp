#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"




namespace Gamma::Analysis
{

    // ports
    // value
    // done


// ports
    // value
    // done


    // Y >= 1 or done(X)  callback()

    struct EnvFollower : public FunctionProcessorPlugin<gam::EnvFollow<double,double>>
    {        
        EnvFollower(double f=10.0) :
         FunctionProcessorPlugin<gam::EnvFollow<double,double>>,
         gam::EnvFollow<double,double>(f)
        {

        }
        enum {
            PORT_VALUE,
            PORT_LAG,
            PORT_DONE,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_LAG: this->lag(v); break;                
            }
        }
        double getPort(int port) {
            switch(port) {
                case PORT_VALUE: return this->value();
                case PORT_DONE: return this->done();
            }
            return 0.0;
        }

        double Tick(double I, double A=1, double X=0.001, double Y=0)
        {        
            return A*(*this)(I);
        }
    };

}