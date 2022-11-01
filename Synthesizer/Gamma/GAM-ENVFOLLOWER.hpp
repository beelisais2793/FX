#pragma once

namespace GAMMA1000
{
    struct EnvFollower : FunctionProcessor
    {
        gam::EnvFollow<double,double> * ef;

        EnvFollower(double cut = 10.0f) : FunctionProcessor() {
            ef = new gam::EnvFollow<double,double>(cut);
            assert(ef != nullptr);
        }
        ~EnvFollower() {
            if(ef) delete ef;
        }

        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            double out = (*ef)(I);
            return out;
        }
        double value() const { return ef->value(); }
        void lag(double v) { ef->lag(v); }
        bool done(double eps=1e-3) { return ef->done(eps); }
    };
}