#pragma once

struct WaveGenerator
{
    double phase;
    double f,sr;
    enum
    {
        Positive,
        Negative,
        Bipolar,
    };
    int Polarity= Positive;

    WaveGenerator(double freq, double sample_rate, int _polarity = Positive) :
    f(freq/sample_rate),
    sr(sample_rate),
    phase(0.0),
    Polarity(_polarity)
    {

    }

    void setFrequency(float freq)
    {
        f = freq/sr;
    }
    virtual double Tick(double I=1, double A=1, double X=1, double Y=1) = 0;
    double polarize(double x) {
        if(Polarity == Positive) return  x;
        if(Polarity == Negative) return -x;
        return 2*x-1;
    }
};

struct SinewaveGenerator : public WaveGenerator
{
    SinewaveGenerator(double freq, double sample_rate, int _polarity = Positive) :
    WaveGenerator(freq,sample_rate, _polarity) {
    
    }
    double Tick(double I=1,double A=1,double X=1, double Y=1) override
    {
        double x = 0.5 + 0.5*std::sin(2*M_PI*std::fmod(I*(f+X) + phase+Y,1));
        phase = std::fmod(phase + f,1);    
        x *= A;    
        return polarize(x);
    }
};
struct TrianglewaveGenerator : public WaveGenerator
{
    TrianglewaveGenerator(double freq, double sample_rate) :
    WaveGenerator(freq,sample_rate) {
    
    }
    double Tick(double I=1,double A=1,double X=1, double Y=1) override
    {
        double x = 0.5 + 0.5*(std::asin(std::sin(2*M_PI*std::fmod(I*(f+X) + phase+Y,1)))/1.5);
        phase = std::fmod(phase + f,1);    
        x *= A;    
        return polarize(x);
    }
};

struct SquarewaveGenerator : public WaveGenerator
{
    
    double duty;
    SquarewaveGenerator(double freq, double sample_rate) :
    WaveGenerator(freq,sample_rate) 
    {
        duty  = 0.5;
    }
    double Tick(double I=1,double A=1,double X=1, double Y=1) override
    {
        double r;
        double d = duty;
        double p = phase;
        phase = std::fmod(phase + X,1);
        duty = std::fmod(duty + Y,1);
        if(phase < duty) r = 0;
        else r = 1;
        phase = p;
        duty  = d;
        phase = std::fmod(phase + f,1);
        return polarize(r);
    }
};
struct SawtoothGenerator : public WaveGenerator
{
    
    SawtoothGenerator(double freq, double sample_rate) :
    WaveGenerator(freq,sample_rate)
    {
        
    }
    double Tick(double I=1,double A=1,double X=1, double Y=1)
    {
        double r = phase;                
        phase = std::fmod(phase + X + Y,1);        
        phase = r;        
        phase = std::fmod(phase + f,1);
        return polarize(r);
    }
};
struct ReverseSawtoothGenerator : public WaveGenerator
{
    
    ReverseSawtoothGenerator(double freq, double sample_rate) :
    WaveGenerator(freq,sample_rate)
    {
        
    }
    double Tick(double I=1,double A=1,double X=1, double Y=1)
    {
        double r = phase;                
        phase = std::fmod(phase + X + Y,1);        
        phase = r;        
        phase = std::fmod(phase + f,1);
        return polarize(1-r);
    }
};


struct dc_block
{
    dc_block(double f, double sr) : _pole(1.0f - (2*M_PI*f/sr))
    {}

    float Tick(float in) {
        y = in - x + _pole * y;
        x = in;
        return y;
    }
    
    void cutoff(double f, double sr)
    {
        _pole = 1.0f - (2 * M_PI * f/sr);
    }

    float _pole;      // pole
    float x = 0.0f;   // delayed input sample
    float y = 0.0f;   // current value
};




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wave 
// It is only a single cycle wave from zero to zero
// WaveCycle is the same thing except it has a filter and a resampler
// It is a single cycle grain so it is very small
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Wave
{
    float * waveTable;
    size_t  tableSize;
    float   f,fs,phase;
    
    Wave(size_t n, float sr) {
        waveTable = new float[tableSize = n];
        fs = sr;
        phase = 0;
        makeSine();        
    }
    ~Wave() {
        if(waveTable) delete [] waveTable;
    }
    void setTable(size_t n, float * wave)
    {
        tableSize = n;
        delete waveTable;
        waveTable = new float[n];
        memcpy(waveTable,wave,n*sizeof(float));
    }
    void addTable(size_t n, float * wave)
    {
        for(size_t i = 0; i < n; i++)
        {
            waveTable[i] = 0.5*(waveTable[i] * wave[i]);
        }
    }
    void setFrequency(float freq) {
        f = freq/fs;
    }
    void setPhase(float p) {
        phase = std::fmod(p,1);
    }
    void makeSine() {        
        float inc   = 1.0f/tableSize;
        for(size_t i = 0; i < tableSize; i++)
        {
            waveTable[i] = std::sin(2.0*M_PI*i*inc);            
        }        
    }
    float operator()(float phaseIn) {
        int   index = int(phaseIn*tableSize) % tableSize;
        float x1 = waveTable[index];
        float frac = x1 - floor(x1);
        int   index2 = (index+1) % tableSize;
        float x2 = waveTable[index2];                
        float out =  x1 + frac*(x2-x1);        
        return out;
    }
    float Inc(float phaseIn) {
        return (*this)(phaseIn);
    }
    float Tick(float I=1, float A=1, float X=0, float Y=0)
    {
        float p = phase;
        //phase = std::fmod(I*(f+X) + Y,1);
        int   index = int(phase*tableSize) % tableSize;
        float x1 = waveTable[index];
        float frac = x1 - floor(x1);
        int   index2 = (index+1) % tableSize;
        float x2 = waveTable[index2];                
        float out = x1 + (frac*(x2-x1));
        phase = std::fmod(phase + f, 1);
        return A*out;
    }    
};

/*
// can sequence multiple wave cycle tables
struct WaveSequence
{
    std::vector<std::shared_ptr<Wave>> waves;
    int cur_index;
    float last_pos = 1;

    WaveSequence() {
        cur_index = 0;
    }
    void addWave(Wave * w)
    {
        waves.push_back( std::shared_ptr<Wave>(w, [](Wave * w) { delete w; }));
    }
    float Tick(float I=1, float A=1, float X = 1, float Y=1)
    {
        if(waves.size() == 0) return 0;
        float out = waves[cur_index].Tick(I,A,X,Y);
        if(last_pos < waves[cur_index].phase) {
            last_pos = 0;
            cur_index = (cur_index+1) % waves.size();
        }
        last_pos = waves[cur_index].phase;
        return out;
    }
};
*/


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WaveVector is a wave table
// the waves are position from 0..127 for midi notes
// It will filter and resample waves put into it.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct WaveVector
{
    std::vector<std::shared_ptr<Wave>> vector;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Multwave for calculating a large number of sine waves with different frequencies/amplitudes
// It's almost the same thing as additive wave except F is a frequency in hertz not a harmonic of f0 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct MultiWave
{
    Wave  * wave;
    float * F;
    float * A;
    float * P;
    size_t  N;
    float fs;
    float f0 = 440.0f;
    float Amp = 1;
    float Offset = 0;
    bool Bipolar = false;

    MultiWave(size_t n, float sr, size_t tableSize=4096)
    {
        wave = new Wave(tableSize, sr);
        wave->makeSine();
        fs = sr;
        F = new float[n];
        A = new float[n];
        P = new float[n];        
        for(size_t i = 0; i < n; i++) {
            F[i] = (i+1);
            A[i] = 0; //float(n-i)/float(n);
            P[i] = 0;
        }
        A[0] = 1.0f;
        f0 = 440.0f;
        N = n;
    }
    ~MultiWave() {
        if(wave) delete wave;
        if(F) delete [] F;
        if(A) delete [] A;
        if(P) delete [] P;
    }
    
    void setF(size_t index, float f)
    {
        F[index] = f;
    }
    void setA(size_t index, float a)
    {
        A[index] = a;
    }
    void setP(size_t index, float p)
    {
        P[index] = p;
    }
    float Tick(float I=1, float Gain=1, float X=0, float Y=0)
    {
        float r = 0;
        for(size_t i = 0; i < N; i++)
        {
            float f = F[i];
            float p = P[i];            
            if(f > fs/2) continue;
            r += A[i]*wave->Inc(p);
            P[i] = fmod(P[i]+f/fs,1);
        }
        float out = Gain*(Offset + Amp*r);
        if(Bipolar) return 2*out-1;
        return out;
    }    
};

///////////////////////////////////////////////////////////////////////
// Additive Wave for calculating many harmonics
///////////////////////////////////////////////////////////////////////
struct AdditiveWave
{
    Wave  * wave;
    float * F;
    float * A;
    float * P;
    size_t  N;
    float fs;
    float f0 = 440.0f;
    float Amp = 1;
    float Offset = 0;
    bool Bipolar = false;

    AdditiveWave(size_t n, float sr, size_t tableSize=4096)
    {
        wave = new Wave(tableSize, sr);
        wave->makeSine();
        fs = sr;
        F = new float[n];
        A = new float[n];
        P = new float[n];        
        for(size_t i = 0; i < n; i++) {
            F[i] = (i+1);
            A[i] = 0; //float(n-i)/float(n);
            P[i] = 0;
        }
        A[0] = 1.0f;
        f0 = 440.0f;
        N = n;
    }
    ~AdditiveWave() {
        if(wave) delete wave;
        if(F) delete [] F;
        if(A) delete [] A;
        if(P) delete [] P;
    }
    void setSine() {
        memset(A,0,sizeof(float)*N);
        A[0] = 1.0f;
        Offset =0;
        Amp = 1;
        Bipolar=false;
    }
    void setSaw() {        
        for(size_t i = 1; i < N; i++)            
        {
            F[i-1] = i;
            A[i-1] = pow(-1,i)/(float)i;            
            P[i-1] = 0;
        }
        Amp = 1;
        Offset=0;
        Bipolar=false;
    }
    void setSquare()
    {
        memset(A,0,sizeof(float)*N);
        for(size_t i = 1; i < N; i+=2)
        {
            F[i-1] = i;
            A[i-1] = (4.0f/M_PI)*1.0f/(float)i;
            P[i-1] = 0;
        }
        Amp = 4.0f/M_PI;
        Offset = 0;
        Bipolar=false;
    }
    void setPulse(float duty)
    {
        memset(A,0,sizeof(float)*N);
        for(size_t i = 1; i < N; i++)
        {
            F[i] = i;
            A[i] = sin(M_PI*(float)i*duty)/(M_PI*(float)i*duty);
            P[i] = 0.25;
        }
        Amp = 2*duty;
        Offset = duty;
        Bipolar=true;
    }
    void setTriangle()
    {                
        memset(A,0,sizeof(float)*N);            
        for(size_t i = 0; i < N; i++)
        {      
            float n = (2*i)+1;       
            F[i] = n;                     
            A[i] = pow(-1.0f,i)/float(n*n);                             
            P[i] = 0;
        }         
        Amp = 8/(M_PI*M_PI);
        Offset = 0;
        Bipolar=false;
    }
    void setRandom()
    {
        for(size_t i = 1; i < N; i++)
            A[i] = noise.rand()/i;
    }
    void setFrequency(float f) {
        f0 = f;
    }
    float Tick(float I=1, float Gain=1, float X=0, float Y=0)
    {
        float r = 0;
        for(size_t i = 0; i < N; i++)
        {
            float f = f0*F[i];
            float p = P[i];            
            if(f > fs/2) continue;
            r += A[i]*wave->Inc(p);
            P[i] = fmod(P[i]+f/fs,1);
        }
        float out = Gain*(Offset + Amp*r);
        if(Bipolar) return 2*out-1;
        return out;
    }    
};

struct WaveIntegrator
{
    float prev = 0;
    float scale = 1.0f/8.0f;
    float Tick(float in)
    {
        prev = 0.995*prev + in;
        return prev * scale;
    }
};

// will be upside down
struct WaveDifferencer
{
    float prev = 0;
    float scale = 1.0f/8.0f;
    float Tick(float in)
    {
        prev = 0.995*prev - in;
        return prev * scale;
    }
};

