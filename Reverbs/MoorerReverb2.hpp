#include "iostream"
#include "fstream"
#include "vector"
#include "limits"
#include "math.h"

#define PI 3.14159265358979323846

#define LOG_INFO(msg) std::cout<<"[INFO] "<<msg<<std::endl;
#define LOG_WARN(msg) std::cout<<"[WARN] "<<msg<<std::endl;
#define LOG_ERROR(msg) std::cout<<"[ERROR] "<<msg<<std::endl;

float min(float a, float b){
    if(a<b){
        return a;
    }
    else{
        return b;
    }
}

float max(float a, float b){
    if(a>b){
        return a;
    }
    else{
        return b;
    }
}

float lerp(float l, float r, float t, float lo=0.0f, float hi=1.0f){

    if(hi == lo){
        return l;
    }

    t = (t-lo)/(hi-lo);

    // interpolate
    return l*(1-t) + r*(t);
}

class GainFilter{
public:
    float gain=1.0f;
public:
    void init(float g=1.0f){
        gain = g;
    }

    void set_gain(float g){
        gain = g;
    }

    float process(float n){
        return n*gain;
    }
};

class DelayFilter{
public:
    std::vector<float> delay_pipe;
    float delay_samples;
    float delay_ms;
    float delay_frac;
    float max_delay_ms;
    int max_delay_samples;
    int w_index;
    int r_index;
public:

    void init(float ms, float delay_ms_max, int sample_rate=44100){

        delay_samples = 0.0f;
        delay_ms = ms;
        max_delay_samples = ceil(sample_rate * delay_ms_max / 1000.0f);
        max_delay_ms = max_delay_samples * 1000.0f / sample_rate;

        // get the delay in sammples and separate out fractional parts
        float delay_raw = sample_rate * ms / 1000.0f;
        delay_samples = floor(delay_raw);
        delay_frac = delay_raw - delay_samples;

        // initialize the delay pipe with 0.0f data and set read write indexes
        r_index = 0;
        w_index = 0;

        delay_pipe.resize(max_delay_samples,0.0f);

    }

    float process(float n){

        float out=0.0f;
        if(delay_samples == 0){
            out = n;
        }
        else{
            out = read_delay();
        }
        write_delay(n);
        return out;
    }

    void set_delay_ms(float ms,int sample_rate=44100){
        
        if(ms > max_delay_ms){
            LOG_ERROR("Delay Filter : exceeded max_delay_ms");
            return;
        }
        // delay
        delay_ms = ms;

        // get the delay in sammples and separate out fractional parts
        float delay_raw = sample_rate * ms / 1000.0f;
        delay_samples = floor(delay_raw);
        delay_frac = delay_raw - delay_samples;

        // read position and wrap around the buffer max size
        r_index = w_index - delay_samples;
        r_index = r_index<0?r_index+max_delay_samples:r_index;
        
    }

    float read_delay(){
        float out = 0.0f;
        out = delay_pipe[r_index];

        int n_minus_one_r_index = r_index - 1;
        n_minus_one_r_index = n_minus_one_r_index < 0? max_delay_samples - 1: n_minus_one_r_index;

        float out_n_minus_one = delay_pipe[n_minus_one_r_index];

        return lerp(out,out_n_minus_one,delay_frac);
    }

    void write_delay(float n){
        
        delay_pipe[w_index++] = n;
        w_index = w_index >= max_delay_samples ? 0 : w_index;

        ++r_index;
        r_index = r_index >= max_delay_samples ? 0 : r_index;

    }

    void reset(){
        delay_pipe.clear();
        delay_pipe.resize(max_delay_samples,0.0f);
        r_index = 0;
        w_index = 0;
        set_delay_ms(delay_ms,44100);
    }

};

class LowPassFilter{
public:
    float coeff=0.0f;
    float n_minus_one=0.0f;
public:

    void init(float lp_coeff,int sample_rate=44100){
        set_coeff(lp_coeff);
    }

    float process(float n){
        n_minus_one = n * (1 - coeff) + (coeff * n_minus_one); // this was the correct one 
        return n_minus_one;
    }

    void set_coeff(float c){
        coeff = c;
    }
};

class CombFilter{
public:
    GainFilter gain_filter;
    DelayFilter delay_filter;
    LowPassFilter low_pass_filter;
public:
    void init(float ms, float delay_ms_max, float g, float lp_coeff, int sample_rate=44100){
        delay_filter.init(ms,delay_ms_max,sample_rate);
        low_pass_filter.init(lp_coeff,sample_rate);
        gain_filter.init(g);
    }
    float process(float n){
        // delay -> gain -> low pass -> feedback -> output
        float delayed_n = delay_filter.read_delay();
        float delayed_n_gain = gain_filter.process(delayed_n);
        float delayed_n_gain_low_pass = low_pass_filter.process(delayed_n_gain);
        float delayed_n_gain_low_pass_feedback = n + delayed_n_gain_low_pass;
        delay_filter.write_delay(delayed_n_gain_low_pass_feedback);
        float out = delayed_n;
        return out;
    }

};

class AllPassFilter{
public:
    float wet_dry_ratio=1.0f;
    DelayFilter delay_filter;
    GainFilter gain_filter;

public:
    void init(float ms, float delay_ms_max, float g, int sample_rate=44100){
        delay_filter.init(ms,delay_ms_max,sample_rate);
        gain_filter.init(g);
    }

    float process(float n){
        float delayed_n = delay_filter.read_delay();
        float delayed_n_gain = gain_filter.process(delayed_n);
        float delayed_n_gain_feedback = n + delayed_n_gain;
        float out = delayed_n + (-1.0f)*(gain_filter.process(delayed_n_gain_feedback));
        delay_filter.write_delay(delayed_n_gain_feedback);
        return out;
    }

};


class Header{
public:
	uint8_t riff_label[4]; // (00) = {'R','I','F','F'}
	uint32_t riff_size; // (04) = 36 + data_size
	uint8_t file_tag[4]; // (08) = {'W','A','V','E'}
	uint8_t fmt_label[4]; // (12) = {'f','m','t',' '}
	uint32_t fmt_size; // (16) = 16
	uint16_t audio_format; // (20) = 1
	uint16_t channel_count; // (22) = 1 or 2
	uint32_t sampling_rate; // (24) = (anything)
	uint32_t bytes_per_second; // (28) = (see above)
	uint16_t bytes_per_sample; // (32) = (see above)
	uint16_t bits_per_sample; // (34) = 8 or 16
	uint8_t data_label[4]; // (36) = {'d','a','t','a'}
	uint32_t data_size; // (40) = # bytes of data
};

class Wave{
public:
    Header header;
    std::vector<std::vector<float>> samples;
    std::vector<float> max_amplitude;
    std::vector<float> min_amplitude;

    int num_samples=0;
    int num_channels=0;

    void read_header(std::string file_path){
        FILE* in = fopen(file_path.c_str(),"rb");
        if(!in){
            LOG_ERROR("read : file does not exsit");
            exit(-1);
        }
        fread(&header,sizeof(Header),1,in);
        fclose(in);
        if(header.channel_count > 1){
            LOG_ERROR("read : only mono tracks supported for now");
            exit(-1);
        }
    }

    void write_header(std::string file_path){
        if(header.channel_count > 1){
            LOG_ERROR("write : only mono tracks supported for now");
            exit(-1);
        }
        FILE* out = fopen(file_path.c_str(),"wb");
        if(!out){
            LOG_ERROR("write : file does not exsit");
            exit(-1);
        }
        fseek(out,0,SEEK_SET);
        fwrite(&header,sizeof(header),1,out);
        fclose(out);
    }

    void print_header(int i){
         	  printf("\n[HEADER]\n");
        if(i) printf("riff_label:       %c%c%c%c.\n",header.riff_label[0],header.riff_label[1],header.riff_label[2],header.riff_label[3]);
        	  printf("riff_size:        %u.\n",header.riff_size);
        if(i) printf("file_tag:         %c%c%c%c.\n",header.file_tag[0],header.file_tag[1],header.file_tag[2],header.file_tag[3]);
        if(i) printf("fmt_label:        %c%c%c%c.\n",header.fmt_label[0],header.fmt_label[1],header.fmt_label[2],header.fmt_label[3]);	
        	  printf("fmt_size:         %u.\n",header.fmt_size);
        	  printf("audio_format:     %u.\n",header.audio_format);
        	  printf("channel_count:    %u.\n",header.channel_count);
        	  printf("sampling_rate:    %u.\n",header.sampling_rate);		  
        	  printf("bytes_per_second: %u.\n",header.bytes_per_second);
        	  printf("bytes_per_sample: %u.\n",header.bytes_per_sample);
        	  printf("bits_per_sample:  %u.\n",header.bits_per_sample);
        if(i) printf("data_label:       %c%c%c%c.\n",header.data_label[0],header.data_label[1],header.data_label[2],header.data_label[3]);
        	  printf("data_size:        %u.\n",header.data_size);
    }

    void read_data(std::string file_path){
        FILE* in = fopen(file_path.c_str(),"rb");
        num_samples = header.data_size/(header.channel_count*header.bits_per_sample/8);
        num_channels = header.channel_count;
        fseek(in,sizeof(Header),SEEK_SET);
        uint8_t* data = new uint8_t[header.data_size];
        fread(data,header.data_size,1,in);
        int16_t* data16 = (int16_t*)(data);
        // clear all channels
        for(int channel=0;channel<(int)samples.size();++channel){
            samples[channel].clear();
        }
        samples.clear();
        // resize buffer and reserve samples
        samples.resize(num_channels);
        for(int channel=0;channel<(int)samples.size();++channel){
            samples[channel].reserve(num_samples);
        }
        // convert PCM data to float samples
        for(int i=0;i<num_samples;++i){
            for(int c=0;c<num_channels;++c){
                if(header.bits_per_sample == 16){
                    float sample = data16[i*num_channels + c]/(float)((1<<15));
                    samples[c].push_back(sample);
                }
                else{
                    LOG_ERROR("read : only 16 bit signed pcm supported");
                    exit(-1);
                }

            }
        }   
        delete[] data;
        fclose(in);
    }

    void write_data(std::string file_path){
        FILE* out = fopen(file_path.c_str(),"ab");
        fseek(out,sizeof(Header),SEEK_SET);
        for(int i=0;i<num_samples;++i){
            for(int c=0;c<num_channels;++c){
                float sample = samples[c][i];
                if(header.bits_per_sample == 16){
                    int16_t bytes_2 = (sample * ((1<<15)));
                    fwrite(&bytes_2,2,1,out);
                }
                else{
                    LOG_ERROR("write : only 16 bit signed pcm supported");
                    exit(-1);
                }
            }
        }
        fclose(out);
    }

    void read_wav(std::string file_path){
        read_header(file_path);
        read_data(file_path);
    }

    void write_wav(std::string file_path){
        write_header(file_path.c_str());
        write_data(file_path.c_str());
    }

    void init_channels_maxmin(){
        // clear it
        max_amplitude.clear();
        min_amplitude.clear();
        // initialize
        for(int i=0;i<num_channels;++i){
            max_amplitude.push_back(std::numeric_limits<float>::min());
            min_amplitude.push_back(std::numeric_limits<float>::max());
        }
    }

    void update_channel_maxmin(){
        for(int i=0;i<num_samples;++i){
            for(int c=0;c<num_channels;++c){
                float sample = samples[c][i];
                //track max min
                if(sample > max_amplitude[c]){
                    max_amplitude[c] = sample;
                }
                if(sample < min_amplitude[c]){
                    min_amplitude[c] = sample;
                }
            }
        }
    }

    void normalize(float min, float max){
        std::vector<float> sum;
        sum.resize(2,0.0f);
        for (int i = 0; i < num_samples; i++){
            for (int c = 0; c < num_channels; c++){
                float x = samples[c][i];
                sum[c] += x;
            }
        }

        for (int i = 0; i < num_samples; i++){
            for (int c = 0; c < num_channels; c++){
                samples[c][i] += sum[c]/num_samples;
                float x = samples[c][i];
                x = ((max-min) * (x - min_amplitude[c]) / (max_amplitude[c] - min_amplitude[c])) - (max-min)/2;
                samples[c][i] = x;
            }
        }
    }

    void info_summary(){
        LOG_INFO("sample_rate : "<<header.sampling_rate);
        LOG_INFO("bit_depth : "<<header.bits_per_sample);
        LOG_INFO("num_channels : "<<num_channels);
        LOG_INFO("num_samples : "<<num_samples);
        LOG_INFO("num_seconds : "<<num_samples/header.sampling_rate);
    }
};
// Recommended values from https://azrael.digipen.edu/MAT321/Moorer.pdf
// Also from Ken Steiglitz page 294

float delays_ms[] = {50.0f, 56.0f, 61.0f, 68.0f, 72.0f, 78.0f};

float g_25khz[] = {0.24f, 0.26f, 0.28f, 0.29f, 0.30f, 0.32f};
float g_50khz[] = {0.46f, 0.48f, 0.50f, 0.52f, 0.53f, 0.55f};

// float g_44_1khz[6];
float g_44_1khz[6] = {};
float lp_coeff[6] = {};

// lerp g from 25 and 50 khz vals in research paper 
void calc_g(){
    for(int i=0;i<6;++i){
        float m = (g_50khz[i] - g_25khz[i]) / 25000.0f;
        g_44_1khz[i] = m*(44100.0f - 25000.0f) + g_25khz[i];
        LOG_INFO("g("<<i<<") : "<<g_44_1khz[i]);
    }
    /*
        [INFO] g(0) : 0.40808
        [INFO] g(1) : 0.42808
        [INFO] g(2) : 0.44808
        [INFO] g(3) : 0.46572
        [INFO] g(4) : 0.47572
        [INFO] g(5) : 0.49572
    */
}

// normalized the comb gain to be 0.83 for low pass one pole coeff
// R/(1-g) = 0.83 
void calc_coeff(){
    for(int i=0;i<6;++i){
        lp_coeff[i] = (1.0f - g_44_1khz[i]) * 0.83f;
        LOG_INFO("lp_coeff("<<i<<") : "<<lp_coeff[i]);
    }
    /*
        [INFO] lp_coeff(0) : 0.491294
        [INFO] lp_coeff(1) : 0.474694
        [INFO] lp_coeff(2) : 0.458094
        [INFO] lp_coeff(3) : 0.443452
        [INFO] lp_coeff(4) : 0.435152
        [INFO] lp_coeff(5) : 0.418552
    */
}


class MoorerReverb{
public:
    CombFilter cf[6];
    AllPassFilter ap[1];

public:
    void init(){
        // number of combs
        int n_cf = sizeof(cf)/sizeof(cf[0]);
        LOG_INFO("n_combs : "<<n_cf);

        // set the comb filter parameters 
        float max_delay_ms = 100.0f;
        for(int i=0;i<n_cf;++i){
            cf[i].init(delays_ms[i],max_delay_ms,g_44_1khz[i],lp_coeff[i],44100);
            cf[i].delay_filter.set_delay_ms(delays_ms[i],44100);
        }

        // set all pass parameters - magic numbers from Mr Moorer
        ap[0].init(6.0f,20.0f,0.7f,44100);
        

    }
    float process(float n){
        float out = 0.0f;
        int n_cf = sizeof(cf)/sizeof(cf[0]);

        // accumulate from combs
        for(int i=0;i<n_cf;++i){
            // normalize for each comb to suppress total
            float cy = cf[i].process(n);
            out = out + cy;
        }

        //out = out + sum/6;

        // process through allpass 
        out = ap[0].process(out);

        return out;
    }
};

void arg_error(){
    LOG_ERROR("usage : ./reverb <input.wav>(required) [output.wav](optional)");
    exit(-1);
}

float db_to_vol(float db){
    return pow(10,db/20);
}

float vol_to_db(float a){
    return 20*log10(a);
}

int main(int argc, char** argv){
    LOG_INFO("--------------");
    LOG_INFO("Moorer Reverb");
    LOG_INFO("--------------");
    std::string input_file;
    std::string output_file;
    if(argc<1){
        arg_error();
    }
    else if(argc == 2){
        input_file = argv[1];
        output_file = "output.wav";
    }
    else if(argc == 3){
        input_file = argv[1];
        output_file = argv[2];
    }else{
        arg_error();
    }
    Wave w;
    w.read_wav(argv[1]);
    w.init_channels_maxmin();
    w.update_channel_maxmin();

    // calculate coefficients 
    calc_g();
    calc_coeff();
    MoorerReverb mr;
    mr.init();

    // do processing here
    for(int i=0; i<w.num_samples; ++i){
        for(int c=0; c<w.num_channels;++c){
            // wet dry mix biasing 
            w.samples[c][i] = w.samples[c][i]*9.0f + mr.process(w.samples[c][i]);
        }
    }

    // normalizing to db 
    LOG_INFO("normalized_volume : "<<db_to_vol(-1.5f));
    LOG_INFO("normalized_volume_db: "<<-1.5f);
    w.update_channel_maxmin();
    w.normalize(-db_to_vol(-1.5f),db_to_vol(-1.5f));
    w.write_wav(output_file);
    LOG_INFO("input_file : "<<input_file);
    LOG_INFO("output_file : "<<output_file);
    w.info_summary();
    return 0;
}


