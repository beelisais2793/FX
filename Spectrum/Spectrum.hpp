#pragma once

namespace Spectrum
{
    template<typename T>
    using complex_vector = sample_vector<std::complex<T>>;

    template<typename T>
    struct Window 
    {   
        sample_vector<T> window;

        Window(size_t i) { window.resize(i); }
        virtual ~Window() = default;

        T& operator[](size_t i) { return window[i]; }

        sample_vector<T> operator * (const sample_vector<T> & v) { return window * v; }
    };
    template<typename T>
    struct Rectangle: public Window<T>
    {
        Rectangle(size_t i) : Window<T>(i) { 
            fill(this->window,1.0f);
            } 
    };
    template<typename T>
    struct Hamming: public Window<T>
    {
        Hamming(size_t n) : Window<T>(n) {            
            for(size_t i = 0; i < this->window.size(); i++)
            {
                this->window[i] = 0.54 - (0.46 * std::cos(2*M_PI*(double)i/(double)n));
            }        
        }
    };
    template<typename T>
    struct Hanning: public Window<T>
    {
        Hanning(size_t n) : Window<T>(n) {            
            for(size_t i = 0; i < this->window.size(); i++)
            {
                this->window[i] = 0.5*(1 - std::cos(2*M_PI*(double)i/(double)n));
            }        
        }
    };
    template<typename T>
    struct Blackman: public Window<T>
    {
        Blackman(size_t n) : Window<T>(n)    
        {            
            for(size_t i = 0; i < this->window.size(); i++)                    
                this->window[i] = 0.42 - (0.5* std::cos(2*M_PI*i/(n)) + (0.08*std::cos(4*M_PI*i/n)));        
        }
    };
    template<typename T>
    struct BlackmanHarris: public Window<T>
    {
        BlackmanHarris(size_t n) : Window<T>(n)    
        {            
            for(size_t i = 0; i < this->window.size(); i++)            
            {   
                double ci = (double) i / (double) n;
                this->window[i] = 0.35875 
                        - 0.48829*std::cos(2*M_PI*(ci))
                        + 0.14128*std::cos(4.0*M_PI*(ci)) 
                        - 0.01168*std::cos(6.0*M_PI*(ci));
            }
        }
    };
    template<typename T>
    struct Gaussian: public Window<T>
    {
        Gaussian(size_t i) : Window<T>(i)
        {
            T a,b,c=0.5;
            for(size_t n = 0; n < this->window.size(); n++)
            {
                a = ((double)n - c*(this->window.size()-1)/(std::sqrt(c)*this->window.size()-1));
                b = -c * std::sqrt(a);
                this->window(n) = std::exp(b);
            }
        }
    };
    template<typename T>
    struct Welch: public Window<T>
    {
        Welch(size_t n) : Window<T>(n)
        {
            for(size_t i = 0; i < this->window.size(); i++)
                this->window[i] = 1.0 - std::sqrt((2.0*(double)i-(double)this->window.size()-1)/((double)this->window.size()));        
        }
    };
    template<typename T>
    struct Parzen: public Window<T>
    {

        Parzen(size_t n) : Window<T>(n)
        {
            for(size_t i = 0; i < this->window.size(); i++)
                this->window[i] = 1.0 - std::abs((2.0*(double)i-this->window.size()-1)/(this->window.size()));        
        }    
    };
    template<typename T>
    struct Tukey: public Window<T>
    {
        Tukey(size_t num_samples, T alpha) : Window<T>(num_samples)
        {            
            T value = (-1*(num_samples/2)) + 1;
            double n2 = (double)num_samples / 2.0;
            for(size_t i = 0; i < this->window.size(); i++)
            {    
                if(value >= 0 && value <= (alpha * (n2))) 
                    this->window[i] = 1.0; 
                else if(value <= 0 && (value >= (-1*alpha*(n2)))) 
                    this->vector.vector[i] = 1.0;
                else 
                    this->vector.vector[i] = 0.5 * (1 + std::cos(M_PI *(((2.0*value)/(alpha*(double)num_samples))-1)))        ;
                value = value + 1;
            }     
        }
    };

    
    ////////////////////////////////////////////////////////////////
    // FFTW Complex 2 Complex
    ////////////////////////////////////////////////////////////////
    struct C2CD
    {
        fftw_complex * in;    
        fftw_complex * out;
        size_t size;
        fftw_plan p;

        enum Direction {
            BACKWARD= FFTW_BACKWARD,
            FORWARD = FFTW_FORWARD,
        };

        C2CD(size_t n, Direction dir = FORWARD) {
            in = fftw_alloc_complex(n);
            out= fftw_alloc_complex(n);        
            size = n;
            p = fftw_plan_dft_1d(n, in, out, dir, FFTW_ESTIMATE);
        }
        ~C2CD() {
            fftw_destroy_plan(p);
            fftw_free(in);
            fftw_free(out);    
        }
        void set_input(complex_vector<double> & input) {
            for(size_t i = 0; i < size; i++) {
                in[i][0] = input[i].real();
                in[i][1] = input[i].imag();
            }
        }
        void set_input(double * buffer) {
            memcpy(in,buffer,2*size*sizeof(double));
        }
        complex_vector<double> get_output() {
            complex_vector<double> r(size);
            for(size_t i = 0; i < size; i++ )
            {
                r[i].real(out[i][0]);
                r[i].imag(out[i][1]);
            }
            return r;
        }
        void get_output(double * output)
        {
            for(size_t i = 0; i < size; i++ )
            {
                output[2*i] = out[i][0];
                output[2*i+1] = out[i][1];
            }
        }
        void get_output(complex_vector<double>&  output)
        {
            if(output.size() != size) output.resize(size);
            for(size_t i = 0; i < size; i++ )
            {
                output[i].real(out[i][0]);
                output[i].imag(out[i][1]);
            }
        }
        void normalize() {
            for(size_t i = 0; i < size; i++) {
                out[i][0] /= (double)size;    
                out[i][1] /= (double)size;
            }
        }
        void Execute() {
            fftw_execute(p);
        }
    };

    struct C2CF
    {
        fftwf_complex * in;    
        fftwf_complex * out;
        size_t size;
        fftwf_plan p;

        enum Direction {
            BACKWARD=FFTW_BACKWARD,
            FORWARD=FFTW_FORWARD,
        };

        C2CF(size_t n, Direction dir = FORWARD) {
            in = fftwf_alloc_complex(n);
            out= fftwf_alloc_complex(n);        
            size = n;
            p = fftwf_plan_dft_1d(n, in, out, dir, FFTW_ESTIMATE);
        }
        ~C2CF() {
            fftwf_destroy_plan(p);
            fftwf_free(in);
            fftwf_free(out);    
        }
        void set_input(complex_vector<float> & input) {
            for(size_t i = 0; i < size; i++) {
                in[i][0] = input[i].real();
                in[i][1] = input[i].imag();
            }
        }
        void set_input(float * buffer) {
            memcpy(in,buffer,2*size*sizeof(float));
        }
        complex_vector<float> get_output() {
            complex_vector<float> r(size);
            for(size_t i = 0; i < size; i++ )
            {
                r[i].real(out[i][0]);
                r[i].imag(out[i][1]);
            }
            return r;
        }
        void get_output(float * output)
        {
            for(size_t i = 0; i < size; i++ )
            {
                output[2*i]   = out[i][0];
                output[2*i+1] = out[i][1];
            }
        }
        void get_output(complex_vector<float>& output)
        {
            if(output.size() != size) output.resize(size);
            for(size_t i = 0; i < size; i++ )
            {
                output[i].real(out[i][0]);
                output[i].imag(out[i][1]);
            }
        }
        void normalize() {
            for(size_t i = 0; i < size; i++) {
                out[i][0] /= (float)size;    
                out[i][1] /= (float)size;
            }
        }
        void Execute() {
            fftwf_execute(p);
        }
    };

    ////////////////////////////////////////////////////////////////
    // FFTW Complex 2 Real
    ////////////////////////////////////////////////////////////////
    struct C2RD
    {
        fftw_complex * in;    
        double * out;
        size_t size;
        fftw_plan p;

        C2RD() {
            in = NULL;
            out = NULL;
            size = 0;
        }
        C2RD(size_t n) {
            init(n);
        }
        ~C2RD() {
            fftw_destroy_plan(p);
            fftw_free(in);
            fftw_free(out);    
        }
        void init(size_t n) {
            if(in != NULL) fftw_destroy_plan(p);
            if(in != NULL) fftw_free(in);
            if(out!= NULL) fftw_free(out);
            in = NULL;
            out = NULL;
            size = n;
            in = fftw_alloc_complex(n);
            out= fftw_alloc_real(n);                    
            p = fftw_plan_dft_c2r_1d(n, in, out, FFTW_ESTIMATE);
        }
        void set_input(complex_vector<double> & input) {
            for(size_t i = 0; i < size; i++) {
                in[i][0] = input[i].real();
                in[i][1] = input[i].imag();
            }
        }
        void set_input(double * buffer) {
            memcpy(in,buffer,2*size*sizeof(double));
        }
        sample_vector<double> get_output() {
            sample_vector<double> r(size);
            memcpy(r.data(),out, size * sizeof(double));
            return r;
        }
        void get_output(double * output)
        {
            for(size_t i = 0; i < size; i++ )
            {
                output[i] = out[i];                
            }
        }
        void get_output( sample_vector<double> & output)
        {
            if(output.size() != size) output.resize(size);
            for(size_t i = 0; i < size; i++ )
            {
                output[i] = out[i];                
            }
        }
        void normalize() {
            for(size_t i = 0; i < size; i++) {
                out[i] /= (double)size;                    
            }
        }
        void Execute() {
            fftw_execute(p);
        }
    };

    struct C2RF
    {
        fftwf_complex * in;    
        float * out;
        size_t size;
        fftwf_plan p;

        C2RF() {
            in = NULL;
            out = NULL;
            size = 0;
        }
        C2RF(size_t n) {
            in = NULL;
            out = NULL;
            size = 0;
            init(n);
        }
        ~C2RF() {
            fftwf_destroy_plan(p);
            fftwf_free(in);
            fftwf_free(out);    
        }
        void init(size_t n) {
            if(in != NULL) fftwf_destroy_plan(p);
            if(in != NULL) fftwf_free(in);
            if(out != NULL) fftwf_free(out);
            in = NULL;
            out = NULL;
            size = n;
            in = fftwf_alloc_complex(n);
            out= fftwf_alloc_real(n);                    
            p = fftwf_plan_dft_c2r_1d(n, in, out, FFTW_ESTIMATE);
        }
        void set_input(complex_vector<float> & input) {
            for(size_t i = 0; i < size; i++) {
                in[i][0] = input[i].real();
                in[i][1] = input[i].imag();
            }
        }
        void set_input(float * buffer) {
            memcpy(in,buffer,2*size*sizeof(float));
        }
        sample_vector<float> get_output() {
            sample_vector<float> r(size);
            memcpy(r.data(),out, size*sizeof(float));
            return r;
        }
        void get_output(float * output)
        {
            for(size_t i = 0; i < size; i++ )
            {
                output[i] = out[i];
            }
        }
        void get_output( sample_vector<float> & output)
        {
            if(output.size() != size) output.resize(size);
            for(size_t i = 0; i < size; i++ )
            {
                output[i] = out[i];
            }
        }
        void normalize()
        {
            for(size_t i = 0; i < size; i++) 
                out[i] /= (float)size;                
        }
        void Execute() {
            fftwf_execute(p);            
        }
    };


    ////////////////////////////////////////////////////////////////
    // FFTW Real 2 Complex
    ////////////////////////////////////////////////////////////////
    struct R2CD
    {
        double       * in;    
        fftw_complex * out;
        size_t size;
        fftw_plan p;

        R2CD() {
            in = NULL;
            out = NULL;
            size= 0;
        }
        R2CD(size_t n) {
            in = NULL;
            out = NULL;
            size= 0;
            init(n);            
        }
        ~R2CD() {
            fftw_destroy_plan(p);
            fftw_free(in);
            fftw_free(out);    
        }
        void init(size_t n) {
            if(in != NULL) fftw_destroy_plan(p);
            if(in != NULL) fftw_free(in);
            if(out != NULL) fftw_free(out);
            in = NULL;
            out = NULL;
            size = n;
            in = fftw_alloc_real(n);
            out= fftw_alloc_complex(n);                    
            p = fftw_plan_dft_r2c_1d(n, in, out, FFTW_ESTIMATE);
        }
        void set_input(sample_vector<double> & input) {
            memcpy(in,input.data(),size*sizeof(double));
        }
        void set_input(double * buffer) {
            memcpy(in,buffer,size*sizeof(double));
        }
        complex_vector<double> get_output() {
            complex_vector<double> r(size);
            for(size_t i = 0; i < size; i++) {
                r[i].real(out[i][0]);
                r[i].imag(out[i][1]);
            }
            return r;
        }
        void get_output(double * output)
        {
            for(size_t i = 0; i < size; i++)
            {
                output[2*i]   = out[i][0];
                output[2*i+1] = out[i][1];
            }
        }
        void get_output(complex_vector<double> & output) {
            if(output.size() != size) output.resize(size);
            for(size_t i = 0; i < size; i++)
            {
                output[i].real(out[i][0]);
                output[i].imag(out[i][1]);
            }            
        }
        void normalize() {
            for(size_t i = 0; i < size; i++) {
                out[i][0] /= (double)size;    
                out[i][1] /= (double)size;
            }
        }
        void Execute() {
            fftw_execute(p);            
        }
    };

    struct R2CF
    {
        float * in;    
        fftwf_complex * out;
        size_t size;
        fftwf_plan p;

        R2CF() {
            in = NULL;
            out = NULL;
            size = 0;
        }
        R2CF(size_t n) {
            in = NULL;
            out = NULL;
            size = 0;
            init(n);            
        }
        ~R2CF() {
            fftwf_destroy_plan(p);
            fftwf_free(in);
            fftwf_free(out);    
        }
        void init(size_t n) {
            if(in != NULL) fftwf_destroy_plan(p);
            if(in != NULL) fftwf_free(in);
            if(out != NULL) fftwf_free(out);
            in = NULL;
            out = NULL;
            size = n;
            in = fftwf_alloc_real(n);
            out= fftwf_alloc_complex(n);                    
            p = fftwf_plan_dft_r2c_1d(n, in, out, FFTW_ESTIMATE);
        }
        void set_input(sample_vector<float> & input) {
            memcpy(in,input.data(),size*sizeof(float));
        }
        void set_input(float * buffer) {
            memcpy(in,buffer,size*sizeof(float));
        }
        complex_vector<float> get_output() {
            complex_vector<float> r(size);
            for(size_t i = 0; i < size; i++) {
                r[i].real(out[i][0]);
                r[i].imag(out[i][1]);
            }
                
            return r;
        }    
        void get_output(float * output)
        {
            for(size_t i = 0; i < size; i++ )
            {
                output[2*i]   = out[i][0];
                output[2*i+1] = out[i][1];
            }
        }
        void get_output( complex_vector<float> & output)
        {
            if(output.size() != size) output.resize(size);
            for(size_t i = 0; i < size; i++ )
            {
                output[i].real(out[i][0]);
                output[i].imag(out[i][1]);
            }
        }
        void normalize() {
            for(size_t i = 0; i < size; i++) {
                out[i][0] /= (float)size;    
                out[i][1] /= (float)size;
            }
        }
        void Execute() {
            fftwf_execute(p);            
        }
    };

    ////////////////////////////////////////////////////////////////
    // FFTW R2R DCT/DST
    ////////////////////////////////////////////////////////////////
    enum R2RKind
    {
        DCTI = FFTW_REDFT00,
        DCTII= FFTW_REDFT01,
        DCTIII=FFTW_REDFT10,
        DCTIV=FFTW_REDFT11,
        DSTI=FFTW_REDFT11,
        DSTII=FFTW_REDFT00,
        DSTIII=FFTW_REDFT10,
        DSTIV=FFTW_REDFT11,
    };

    struct R2RD
    {
        double       * in;    
        double       * out;
        size_t size;
        fftw_plan p;

        R2RD() {
            in = out = NULL;
            size = 0;
        }
        R2RD(size_t n, R2RKind type = DCTI) {
            init(n,type);
        }
        ~R2RD() {
            fftw_destroy_plan(p);
            fftw_free(in);
            fftw_free(out);    
        }
        void init(size_t n, R2RKind type) {
            if(in != NULL) fftw_destroy_plan(p);
            if(in != NULL) fftw_free(in);
            if(out != NULL) fftw_free(out);
            in = out = NULL;            
            size = n;            
            in = fftw_alloc_real(n);
            out= fftw_alloc_real(n);                    
            p = fftw_plan_r2r_1d(n, in, out, (fftw_r2r_kind)type, FFTW_ESTIMATE);
        }
        void set_input(sample_vector<double> & input) {
            memcpy(in,input.data(),size*sizeof(double));
        }
        void set_input(double * buffer) {
            memcpy(in,buffer,size*sizeof(double));
        }
        sample_vector<double> get_output() {
            sample_vector<double> r(size);
            for(size_t i = 0; i < size; i++) {
                r[i] = out[i];                
            }
            return r;
        }
        void get_output(double * output)
        {
            for(size_t i = 0; i < size; i++ )
            {
                output[i] = out[i];                
            }
        }
        void get_output( sample_vector<double> & output)
        {
            if(output.size() != size) output.resize(size);
            for(size_t i = 0; i < size; i++ )
            {
                output[i] = out[i];                
            }
        }
        void normalize() {
            for(size_t i = 0; i < size; i++)
                out[i] /= (double)size;    
        }
        void Execute() {
            fftw_execute(p);            
        }
    };

    struct R2RF
    {
        float * in;    
        float * out;
        size_t size;
        fftwf_plan p;

        R2RF() {
            in = out = NULL;
            size = 0;
        }
        R2RF(size_t n, R2RKind type = DCTI) {
            in = out = NULL;
            size = 0;
            init(n,type);            
        }
        ~R2RF() {
            fftwf_destroy_plan(p);
            fftwf_free(in);
            fftwf_free(out);    
        }
        void init(size_t n, R2RKind type) {
            if(in != NULL) fftwf_destroy_plan(p);
            if(in != NULL) fftwf_free(in);
            if(out != NULL) fftwf_free(out);
            in = out = NULL;            
            size = n;            
            in = fftwf_alloc_real(n);
            out= fftwf_alloc_real(n);                    
            p = fftwf_plan_r2r_1d(n, in, out, (fftw_r2r_kind)type, FFTW_ESTIMATE);
        }
        void set_input(sample_vector<float> & input) {
            memcpy(in,input.data(),size*sizeof(float));
        }
        void set_input(float * buffer) {
            memcpy(in,buffer,size*sizeof(float));
        }
        sample_vector<float> get_output() {
            sample_vector<float> r(size);
            for(size_t i = 0; i < size; i++) {
                r[i] = out[i];                
            }                
            return r;
        }
        void get_output(float * output)
        {
            for(size_t i = 0; i < size; i++ )
            {
                output[i] = out[i];                
            }
        }
        void get_output( sample_vector<float> & output)
        {
            if(output.size() != size) output.resize(size);
            for(size_t i = 0; i < size; i++ )
            {
                output[i] = out[i];                
            }
        }
        void normalize() {
            for(size_t i = 0; i < size; i++)
                out[i] /= (float)size;    
        }    
        void Execute() {
            fftwf_execute(p);            
        }
    };


    ////////////////////////////////////////////////////////////////
    // FFTW Convolution
    ////////////////////////////////////////////////////////////////    
    sample_vector<float> FFTW_Convolution(sample_vector<float> x, sample_vector<float> y) {
        int M = x.size();
        int N = y.size();       
        float in_a[M+N-1];
        complex_vector<float> out_a[M+N-1];
        float in_b[M+N-1];
        complex_vector<float> out_b[M+N-1];
        complex_vector<float> in_rev[M+N-1];
        sample_vector<float> out_rev(M+N-1);

        // Plans for forward FFTs
        fftwf_plan plan_fwd_a = fftwf_plan_dft_r2c_1d (M+N-1, in_a, reinterpret_cast<fftwf_complex*>(&out_a), FFTW_MEASURE);
        fftwf_plan plan_fwd_b = fftwf_plan_dft_r2c_1d (M+N-1, in_b, reinterpret_cast<fftwf_complex*>(&out_b), FFTW_MEASURE);

        // Plan for reverse FFT
        fftwf_plan plan_rev = fftwf_plan_dft_c2r_1d (M+N-1,reinterpret_cast<fftwf_complex*>(&in_rev), out_rev.data(), FFTW_MEASURE);

        // Prepare padded input data
        std::memcpy(in_a, x.data(), sizeof(float) * M);
        std::memset(in_a + M, 0, sizeof(float) * (N-1));
        std::memset(in_b, 0, sizeof(float) * (M-1));
        std::memcpy(in_b + (M-1), y.data(), sizeof(float) * N);
        
        // Calculate the forward FFTs
        fftwf_execute(plan_fwd_a);
        fftwf_execute(plan_fwd_b);

        // Multiply in frequency domain
        for( int idx = 0; idx < M+N-1; idx++ ) {
            in_rev[idx] = out_a[idx] * out_b[idx];
        }

        // Calculate the backward FFT
        fftwf_execute(plan_rev);

        // Clean up
        fftwf_destroy_plan(plan_fwd_a);
        fftwf_destroy_plan(plan_fwd_b);
        fftwf_destroy_plan(plan_rev);

        return out_rev;
    }

    void blockconvolve(sample_vector<float> h, sample_vector<float> x, sample_vector<float>& y, sample_vector<float> & ytemp)    
    {
        int i;
        int M = h.size();
        int L = x.size();
        y = FFTW_Convolution(h,x);      
        for (i=0; i<M; i++) {
            y[i] += ytemp[i];                     /* add tail of previous block */
            ytemp[i] = y[i+L];                    /* update tail for next call */
        }        
    }


    ////////////////////////////////////////////////////////////////
    // FFTW Deconvolution
    ////////////////////////////////////////////////////////////////
    sample_vector<float> FFTW_Deconvolution(sample_vector<float> & xin, sample_vector<float> & yout)
    {
        int M = xin.size();
        int N = yout.size();
        float x[M] = {0,1,0,0};
        float y[N] = {0,0,1,0,0,0,0,0};
        float in_a[M+N-1];
        std::complex<float> out_a[M+N-1];
        float in_b[M+N-1];
        std::complex<float> out_b[M+N-1];
        std::complex<float> in_rev[M+N-1];
        sample_vector<float> out_rev(M+N-1);

        // Plans for forward FFTs
        fftwf_plan plan_fwd_a = fftwf_plan_dft_r2c_1d (M+N-1, in_a, reinterpret_cast<fftwf_complex*>(&out_a), FFTW_MEASURE);
        fftwf_plan plan_fwd_b = fftwf_plan_dft_r2c_1d (M+N-1, in_b, reinterpret_cast<fftwf_complex*>(&out_b), FFTW_MEASURE);

        // Plan for reverse FFT
        fftwf_plan plan_rev = fftwf_plan_dft_c2r_1d (M+N-1,reinterpret_cast<fftwf_complex*>(&in_rev[0]), out_rev.data(), FFTW_MEASURE);

        // Prepare padded input data
        std::memcpy(in_a, xin.data(), sizeof(float) * M);
        std::memset(in_a + M, 0, sizeof(float) * (N-1));
        std::memset(in_b, 0, sizeof(float) * (M-1));
        std::memcpy(in_b + (M-1), yout.data(), sizeof(float) * N);
        
        // Calculate the forward FFTs
        fftwf_execute(plan_fwd_a);
        fftwf_execute(plan_fwd_b);

        // Multiply in frequency domain
        for( int idx = 0; idx < M+N-1; idx++ ) {
            in_rev[idx] = out_a[idx] / out_b[idx];
        }

        // Calculate the backward FFT
        fftwf_execute(plan_rev);

        // Clean up
        fftwf_destroy_plan(plan_fwd_a);
        fftwf_destroy_plan(plan_fwd_b);
        fftwf_destroy_plan(plan_rev);

        return out_rev;
    }

    ////////////////////////////////////////////////////////////////
    // FFTW Cross Correlation
    ////////////////////////////////////////////////////////////////
    sample_vector<float> FFTW_CrossCorrelation(sample_vector<float> & xin, sample_vector<float> & yout)
    {
        int M = xin.size();
        int N = yout.size();        
        float in_a[M+N-1];
        std::complex<float> out_a[M+N-1];
        float in_b[M+N-1];
        std::complex<float> out_b[M+N-1];
        std::complex<float> in_rev[M+N-1];
        sample_vector<float> out_rev(M+N-1);

        // Plans for forward FFTs
        fftwf_plan plan_fwd_a = fftwf_plan_dft_r2c_1d (M+N-1, in_a, reinterpret_cast<fftwf_complex*>(&out_a), FFTW_MEASURE);
        fftwf_plan plan_fwd_b = fftwf_plan_dft_r2c_1d (M+N-1, in_b, reinterpret_cast<fftwf_complex*>(&out_b), FFTW_MEASURE);

        // Plan for reverse FFT
        fftwf_plan plan_rev = fftwf_plan_dft_c2r_1d (M+N-1,reinterpret_cast<fftwf_complex*>(&in_rev[0]), out_rev.data(), FFTW_MEASURE);

        // Prepare padded input data
        std::memcpy(in_a, xin.data(), sizeof(float) * M);
        std::memset(in_a + M, 0, sizeof(float) * (N-1));
        std::memset(in_b, 0, sizeof(float) * (M-1));
        std::memcpy(in_b + (M-1), yout.data(), sizeof(float) * N);
        
        // Calculate the forward FFTs
        fftwf_execute(plan_fwd_a);
        fftwf_execute(plan_fwd_b);

            // Multiply in frequency domain
        for( int idx = 0; idx < M+N-1; idx++ ) {
            in_rev[idx] = std::conj(out_a[idx]) * out_b[idx]/(float)(M+N-1);
        }

        // Calculate the backward FFT
        fftwf_execute(plan_rev);

        // Clean up
        fftwf_destroy_plan(plan_fwd_a);
        fftwf_destroy_plan(plan_fwd_b);
        fftwf_destroy_plan(plan_rev);

        return out_rev;
    }

    

    ////////////////////////////////////////////////////////////////
    // FFTW Resampler
    ////////////////////////////////////////////////////////////////

    struct FFTResampler
    {
        int inFrameSize;
        int inWindowSize;
        int inSampleRate;
        float *inWindowing;
        fftwf_plan inPlan;
        int outFrameSize;
        int outWindowSize;
        int outSampleRate;
        float *outWindowing;
        fftwf_plan outPlan;
        float *inFifo;
        float *synthesisMem;
        fftwf_complex *samples;
        int pos;
        

        FFTResampler(size_t inSampleRate, size_t outSampleRate, size_t nFFT)
        {
            
            pos = 0;
            if (outSampleRate < inSampleRate) {
                nFFT = nFFT * inSampleRate * 128 / outSampleRate;
            }
            else {
                nFFT = nFFT * outSampleRate * 128 / inSampleRate;
            }
            nFFT += (nFFT % 2);

            inFrameSize = nFFT;
            inWindowSize = nFFT * 2;
            inSampleRate = inSampleRate;
            outSampleRate = outSampleRate;
            outFrameSize = inFrameSize * outSampleRate / inSampleRate;
            outWindowSize = outFrameSize * 2;        

            outWindowing = (float *) fftwf_alloc_real(outFrameSize);
            inFifo = (float *) fftwf_alloc_real(std::max(inWindowSize, outWindowSize));
            samples = (fftwf_complex *) fftwf_alloc_complex(std::max(inWindowSize, outWindowSize));
            inWindowing = (float *) fftwf_alloc_real(inFrameSize);
            synthesisMem = (float *) fftwf_alloc_real(outFrameSize);
                    
            inPlan = fftwf_plan_dft_r2c_1d(inWindowSize,inFifo,samples,FFTW_ESTIMATE);        
            outPlan = fftwf_plan_dft_c2r_1d(outWindowSize,samples,synthesisMem,FFTW_ESTIMATE);
            
            if ((inFifo == NULL) || (inPlan == NULL) || (outPlan == NULL)
                || (samples == NULL)
                || (synthesisMem == NULL) || (inWindowing == NULL) || (outWindowing == NULL)
                ) {
                    assert(1==0);
            }
            float norm = 1.0f / inWindowSize;
            for (size_t i = 0; i < inFrameSize; i++) {
                double t = std::sin(.5 * M_PI * (i + .5) / inFrameSize);
                inWindowing[i] = (float) std::sin(.5 * M_PI * t * t) * norm;
            }
            for (size_t i = 0; i < outFrameSize; i++) {
                double t = std::sin(.5 * M_PI * (i + .5) / outFrameSize);
                outWindowing[i] = (float) std::sin(.5 * M_PI * t * t);
            }    
        }
        
        ~FFTResampler()
        {   
            if (inFifo) {
                free(inFifo);
                inFifo = NULL;
            }

            if (inPlan) {
                fftwf_destroy_plan(inPlan);
                inPlan = NULL;
            }

            if (outPlan) {
                fftwf_destroy_plan(outPlan);
                outPlan = NULL;
            }

            if (samples) {
                fftw_free(samples);
                samples = NULL;
            }

            if (synthesisMem) {
                fftw_free(synthesisMem);
                synthesisMem = NULL;
            }

            if (inWindowing) {
                fftw_free(inWindowing);
                inWindowing = NULL;
            }

            if (outWindowing) {
                fftw_free(outWindowing);
                outWindowing = NULL;
            }    
        }

        void reset()
        {        
            pos = 0;
        }

        

        int Process(const float *input, float *output)
        {
            if ((input == NULL) || (output == NULL)) {
                return -1;
            }
            float *inFifo = inFifo;
            float *synthesis_mem = synthesisMem;
            for (size_t i = 0; i < inFrameSize; i++) {
                inFifo[i] *= inWindowing[i];
                inFifo[inWindowSize - 1 - i] = input[inFrameSize - 1 - i] * inWindowing[i];
            }
            fftwf_execute(inPlan);
            if (outWindowSize < inWindowSize) {
                int half_output = (outWindowSize / 2);
                int diff_size = inWindowSize - outWindowSize;
                memset(samples + half_output, 0, diff_size * sizeof(fftw_complex));
            }
            else if (outWindowSize > inWindowSize) {
                int half_input = inWindowSize / 2;
                int diff_size = outWindowSize - inWindowSize;
                memmove(samples + half_input + diff_size, samples + half_input,
                        half_input * sizeof(fftw_complex));
                memset(samples + half_input, 0, diff_size * sizeof(fftw_complex));
            }
            fftwf_execute(outPlan);
            for (size_t i = 0; i < outFrameSize; i++) {
                output[i] = inFifo[i] * outWindowing[i] + synthesis_mem[i];
                inFifo[outWindowSize - 1 - i] *= outWindowing[i];
            }
            memcpy(synthesis_mem, inFifo + outFrameSize, outFrameSize * sizeof(float));
            memcpy(inFifo, input, inFrameSize * sizeof(float));
            if (pos == 0) {
                pos++;
                return 0;
            }
            pos++;
            return 1;
        }
    };

    
    ////////////////////////////////////////////////////////////////
    // AudioFFT
    ////////////////////////////////////////////////////////////////
    complex_vector<float> audiofft_forward(sample_vector<float> & in)
    {
        audiofft::AudioFFT fft;
        size_t s = in.size();
        if(s % 2 != 0)
        {
            s = (size_t)std::pow(2,std::log((double)s)+1.0);
        }
        sample_vector<float> temp(s);
        sample_vector<float> real(s);
        sample_vector<float> imag(s);
        zeros(temp);
        zeros(real);
        zeros(imag);
        memcpy(temp.data(),in.data(),in.size()*sizeof(float));
        fft.init(in.size());
        fft.fft(temp.data(),real.data(),imag.data());
        Spectrum::complex_vector<float> out(s);
        for(size_t i = 0; i < s; i++)
        {
            out[i].real(real[i]);
            out[i].imag(imag[i]);
        }
        return out;
    }

    sample_vector<float> audiofft_inverse(Spectrum::complex_vector<float> & in)
    {
        audiofft::AudioFFT fft;
        size_t s = in.size();
        if(s % 2 != 0)
        {
            s = (size_t)std::pow(2,std::log((double)s)+1.0);
        }
        sample_vector<float> temp(s);
        sample_vector<float> real(s);
        sample_vector<float> imag(s);
        zeros(temp);
        zeros(real);
        zeros(imag);
        for(size_t i = 0; i < in.size(); i++)
        {
            real[i] = in[i].real();
            imag[i] = in[i].imag();
        }
        fft.init(in.size());
        fft.ifft(temp.data(),real.data(),imag.data());        
        return temp;
    }

    ////////////////////////////////////////////////////////////////
    // FFTConvolver
    ////////////////////////////////////////////////////////////////
    sample_vector<float> fftconvolver(sample_vector<float>& in, size_t block_size, sample_vector<float> & ir)
    {
        sample_vector<float> out(in.size());
        fftconvolver::FFTConvolver conv;
        conv.init(block_size,ir.data(),ir.size());
        conv.process(in.data(),out.data(),in.size());
        return out;
    }
}



