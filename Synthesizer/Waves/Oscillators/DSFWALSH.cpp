
// pwm modulation theory
// varying duty cyrcles
struct Walsh
{
    std::vector<double> duty_table;
    int duty_ctr;
    Oscillators::BlitSquare square;

    Walsh(int n) {
        duty_table.resize(n);
        for(size_t i = 0; i < n; i++)
            duty_table[i] = (double)i / (double)n;
    }
};

// Gamma DSF
// DSF C Project
// Internet DSF Formulas
struct DSF
{
    double phase = 0;
    double freq  = 440.0;
    double inc   = 0;
    int    M,N;

    DSF() {
        M = 2;
        N = 10;
        inc = freq/sampleRate;
    }
    void setFrequency(double f) {
        freq = f;
        inc = f/sampleRate;
    }
    double Tick(double a)
    {
        
        double numerator;
        double theta = 2*M_PI*phase;
        double beta  = M*theta;
        double denom = 1 + a*a - 2 * a * std::cos(beta);

        phase = std::fmod(phase + inc,1);
        if(denom != 0.0)
            numerator = std::sin(theta) - a*std::sin(theta-beta) - std::pow(a,N+1)*(std::sin(theta + N*beta + beta) - a*std::sin(theta+N*beta));
        else
            numerator = a*std::sin(theta);
        return numerator/denom;
    }
};

