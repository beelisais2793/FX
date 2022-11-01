#pragma once

namespace Analog::Filters::Moog::MoogCat
{
    struct MoogCat
    {
        
        double fc = 800.0f; //cutoff frequency
        double res = 0.0f; //resonance
        double Fs = 44100.0f; //sampling frequency
        double g = tan(M_PI * fc / Fs); //frequency warping factor
        double r_cat = 1.064f; //filter type value    

        //vector of states
        static constexpr size_t numStates = 4;
        std::vector<std::array<double, numStates>> state;

        MoogCat() {

        }
        void setCutoff(double f) {
            fc = f;        
        }
        void setResonance(double r) {
            res = r;
        }

        double processSample(double input, size_t channel) noexcept {
            auto& s = state[channel];
            const double g_2 = g*g;
            const double g_3 = g_2*g;
            const double g_4 = g_3*g;
            const double r_cat_2 = r_cat*r_cat;
            const double val1 = res*g_4*r_cat_2;
            const double val2 = g_3*r_cat;
            const double val3 = g*r_cat;
            const double val4 = g_2*r_cat_2;
            const double val5 = 2*val3 + 1;
            const double val6 = g_2 + val5;
            const double val7 = g*val6;
            const auto den = (4*val1 + g_4 + 4*val2 + 4*val4 + 2*g_2 + 4*val3 + 1);
            double out = (g_3*s[0] - g_2*(val5)*s[1] + val7*s[2] - (2*val2 + 4*val4 + g_2 + 4*val3 + 1)*s[3])/den;
            const double a = -(val1*4 + g_4 + 4*val2 + 4*val4 - 1)*s[0] + 2*g*(res*val4*4 + val6)*s[1] - 8*g_2*res*r_cat_2*s[2] + 8*g*res*r_cat_2*(2*val3+1)*s[3] + val7*2*input;
            const double b = - 2*val7*s[0] + (-val1*4 - g_4 + 4*val4 + 4*val3 + 1)*s[1] + 8*g_3*res*r_cat_2*s[2] - 8*g_2*res*r_cat_2*(2*val3+1)*s[3] - g_2*(val6)*2*input;
            const double c = 2*g_2*s[0] - g*(val5)*2*s[1] - (val1*4 + g_4 + 4*g_3*r_cat + 4*val4 - 1)*s[2] + 2*g*(res*val4*4 + val6)*s[3] + 2*g_3*input;
            const double d = -2*g_3*s[0] + g_2*(val5)*2*s[1] - val7*2*s[2] + (-val1*4 - g_4 + 4*val4 + 4*val3 + 1)*s[3] - 2*g_4*input;
            s[0] = a/den;
            s[1] = b/den;
            s[2] = c/den;
            s[3] = d/den;
            return out;
        }

        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            double F = fc;
            double R = res;

            setCutoff(F*fabs(X));
            setResonance(R*fabs(Y));
            double out = processSample(I,0);
            return A*out;
        }
    };
}
