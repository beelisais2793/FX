#pragma once

// Interpolators
// Ideal Bandlimited Interpolation
// Linear
// Cubic
// Hermite
// Spline/Bezier
// Lagrange
// Sinc
// FIR

struct LinearInterpolator
{
    double factor;

    LinearInterpolator(double fac) : factor(fac) 
    {

    }

    std::vector<float> Interpolate(size_t n, float * in)
    {
        std::vector<float> r(n*factor);
        for(size_t i = 0; i < n; i++)
        {
            r[i] = in[i];
            for(size_t j = 1; j < (size_t)factor; j++)
            {
                r[i+j] = (double)j/((double)j/(double)factor)*in[i];
            }
        }
        return r;
    }
};