#pragma once


template<int N>
struct LFO9000
{
    FunctionLFO *LFO[N];
    float output[N];
    LFO9000()
    {
        for(size_t i = 0; i < N; i++)
            LFO[i] = new FunctionLFO();
    }
    ~LFO9000()
    {
        for(size_t i = 0; i < N; i++)
            delete LFO[i];
    }
    void setWaveForm(FunctionLFO::Waveform wave, int channel = -1)
    {
        if(channel < 0) {
            for(size_t i = 0; i < N; i++)
                LFO[i]->setWaveForm(wave);
        }
    }
    float operator()(size_t channel) {
        return output[channel];
    }
    void Tick(float I=1, float A=1, float X=0, float Y=0) {
        for(size_t i = 0; i < N; i++)
            output[i] = LFO[i]->Tick(I,A,X,Y);
    }
    float Sum() {
        float sum = 0;
        for(size_t i = 0; i < N; i++)
            sum += output[i];
        return sum/N;
    }
    float Accumulate(std::initializer_list<size_t> & index) {
        float sum = 0;
        for(auto i = index.begin(); i!= index.end(); i++)
            sum += output[*i];
        return sum/index.size();
    }
    float Modulate() {
        float sum = 1;
        for(size_t i = 0; i < N; i++)
            sum *= output[i];
        return sum;
    }
    float Modulate(std::initializer_list<size_t> & index) {
        float sum = 1;
        for(auto i = index.begin(); i!= index.end(); i++)
            sum *= output[*i];
        return sum;
    }
    float Circulate() {
        float sum = output[0];
        for(size_t i = 1; i < N; i++)
            sum = fmod(sum,output[i]);
        return sum;
    }
    float Circulate(std::initializer_list<size_t> & index) {
        float sum = output[0];
        for(auto i = index.begin(); i!= index.end(); i++)
            sum = fmod(sum,output[*i]);
        return sum;
    }
    float Row(int row) {
        float sum = 0;
        int   X = N/8;
        int   Y = N/4;
        for(size_t i = 0; i < Y; i++)
            sum += output[row*Y + i]
        return sum/Y;
    }
    float Col(int col) {
        float sum = 0;
        int   X = N/8;
        int   Y = N/4;
        for(size_t i = 0; i < X; i++)
            sum += output[i*Y + col]
        return sum/X;
    }
    float Random() {
        Random r;
        return output[r.randint(0,N)];
    }
    float RandomSum() {
        float sum = 0;
        Random r;
        std::vector<int> index;
        for(size_t i = 0; i < N; i++)
            if(r.frand() < 0.5) index.push_back(i);
        for(size_t i = 0; i < index.size(); i++ )
        {
            sum += output[index[i]];            
        }
        return sum;
    }
};