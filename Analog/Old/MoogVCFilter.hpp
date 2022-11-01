#pragma once

namespace Moog::MoogVCF2
{
    class FirstOrderFilter {
    public:
        // Using member intializer list to initialize the reference member variables
        FirstOrderFilter(float& a1, float& b0, float& b1, float& g) : a1(a1), b0(b0), b1(b1), g(g) {}
        float process(float in);
    private:
        // Using reference_wrapper since it is more flexible 
        // and allows the object to be copied if necessary.
        std::reference_wrapper<float> a1;
        std::reference_wrapper<float> b0;
        std::reference_wrapper<float> b1;
        std::reference_wrapper<float> g;
        
        // Make sure previous input and output are initialized.

        // x[n-1]
        float previousInput = 0.0f;
        
        // y[n-1]
        float previousOutput = 0.0f;
    };

    float FirstOrderFilter::process(float in)
    {
        float out = ((b0*in + b1*previousInput - a1*previousOutput) * g) + a1*previousOutput;

        previousInput = in;
        previousOutput = out;

        return out;
    }

    class MoogVCF {
    public:
        MoogVCF();
        float process(float in);
        void calculate_coefficients(float sampleRate, float frequencyHz, float resonance);
        void set_filter_type(int type);
    private:
        // Coefficients (probably don't need to be initialized here.)
        float a1 = 0.0f;
        float b0 = 0.0f;
        float b1 = 0.0f;
        float g  = 0.0f;

        std::vector<FirstOrderFilter> filters;

        // Fixed by design
        const float gComp = 0.5f;
        float gRes = 0.75f;

        float delayed_filter_output = 0.0f;

        // Using an enum to improve readability
        enum filter_type { two_pole_low_pass,  four_pole_low_pass,
                        two_pole_band_pass, four_pole_band_pass,
                        two_pole_high_pass, four_pole_high_pass };

        filter_type filter_t = four_pole_low_pass;

        std::array<std::array<float, 5>, 6> filter_weights;
    };

    MoogVCF::MoogVCF()
    {
        // Create four first-order filters and add them to the vector
        for (int i = 0; i < 4; ++i)
            filters.push_back({a1, b0, b1, g});

        filter_weights[two_pole_low_pass]   = { {0,  0,  1,  0, 0} };
        filter_weights[two_pole_band_pass]  = { {0,  2, -2,  0, 0} };
        filter_weights[two_pole_high_pass]  = { {1, -2,  1,  0, 0} };

        filter_weights[four_pole_low_pass]  = { {0,  0,  0,  0, 1} };
        filter_weights[four_pole_band_pass] = { {0,  0,  4, -8, 4} };
        filter_weights[four_pole_high_pass] = { {1, -4,  6, -4, 1} };
    }

    void MoogVCF::set_filter_type(int type)
    {
        // Convert integer to the enumeration type
        filter_t = static_cast<filter_type>(type);
    }

    void MoogVCF::calculate_coefficients(float sampleRate, float frequencyHz, float resonance)
    {
        a1 = 1.0f;
        b0 = 1.0f / 1.3f;
        b1 = 0.3f / 1.3f;

        // Angular frequency
        float w =  2.0f * M_PI * frequencyHz / sampleRate;

        // Polynomial model which improves the fit
        g = 0.9892f*w - 0.4342f*powf(w, 2.0f) + 0.1381f*powf(w, 3.0f) - 0.0202f*powf(w, 4.0f);

        // Calculate a cubic polynomial to correct for the error
        gRes = resonance * (1.0029f + 0.0526f*w - 0.0926f*powf(w, 2.0f) + 0.0218f*powf(w, 3.0f));
    }

    float MoogVCF::process(float in)
    {
        float feedback_loop = in - ( (delayed_filter_output - (in * gComp)) * gRes * 4.0f );

        // Use the hyperbolic tangent function to approximate the nonlinearity of an analog circuit
        float nonlinearity = tanhf (feedback_loop);

        float filter_out = 0.0f;
        float summing_junction = 0.0f;

        // Use filter_type to index into the two dimensional array
        for (unsigned int i = 0; i < filters.size(); ++i) {
            if (i == 0) {
                summing_junction += nonlinearity * filter_weights[filter_t][i];
                filter_out = filters[i].process(nonlinearity);
            }
            else {
                summing_junction += filter_out * filter_weights[filter_t][i];
                filter_out = filters[i].process(filter_out);
            }
        }

        // This happens just before the multiplication by the coefficient E
        delayed_filter_output = filter_out;

        // Add the final weighting of the output outside of the for loop
        summing_junction += filter_out * filter_weights[filter_t][4];

        return summing_junction;
    }
}