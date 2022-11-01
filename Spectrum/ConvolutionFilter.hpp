// Dissect/Convolution/ConvolutionFilter
#pragma once

namespace Spectrum::ConvolutionFilter
{
    class FastFourierTransformer {

    // class methods
        
    public:
        
        FastFourierTransformer (int fftSize);	//constructor
        ~FastFourierTransformer();				//destructor
        
        
        //fft conversion
        
        void processForward	   (float* channelData, fftw_complex* fftData, int dataSize, int fftSize);
        void processBackward   (fftw_complex* fftData, float* channelData, int fftSize);
        
        // fft mult
        fftw_complex* mult  (fftw_complex* X, fftw_complex* Y, int fftSize);
                            
    // member variables
        
    private:
        
        fftw_complex    *data, *fft_result, *ifft_result,;	
        fftw_plan       plan_forward, plan_backward;
        int             i;
        
    };
    struct ConvolutionFilter
    {
        int filterLength, nfft, convSize, nbFilters, dryBufferSize ;
        float *filterSamples, *ifftSamples ;
        float **olaBuffer ; // 2D overlap Add buffer
        float *dryBuffer ; // 1D circular buffer for LF
        float oldwet, wet;
        FastFourierTransformer *fft ;
        fftw_complex *fftData, *fftDataFilt, *fftH0, *fftH1 ;

        ConvolutionFilter() {
            // The samplesPerBlock is an estimation of the average nb of samples per buffer
            nbFilters = 2 ; // number of filters (1 for mono, 2 for stereo)
            filterLength = 2048;	 // 2048 over 44100 is about 50ms length
            filterSamples = new float[filterLength];
            
            // NFFT for convolution
            if (samplesPerBlock>filterLength) {
            nfft=samplesPerBlock *2 ; 
            }
            else {
                nfft = filterLength *2 ;
            }
            
            // 2D ola Buffer
            convSize = filterLength + samplesPerBlock - 1 ;
            olaBuffer = new float*[nbFilters] ;
            for (int k=0; k<nbFilters; k++) {
                olaBuffer[k] = new float[nfft];
                for (int i=0; i<nfft; i++) {
                    olaBuffer[k][i] = 0;
                }
            }
            
            // circular buffer for the dry signal to account for latency
            dryBufferSize = filterLength/2 + samplesPerBlock ;
            dryBuffer = new float[dryBufferSize] ; // only works if samplesPerBlock remains stable
            for (int i=0 ; i<dryBufferSize ; i++) {
                dryBuffer[i] = 0 ;
            }
            
            // Plan initialisation
            if (fft == NULL){
                fft = new FastFourierTransformer(nfft) ;
            }	
            ifftSamples = new float[nfft];
            fftData = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nfft) ; // will be used to store the unfiltered FFT of the incoming signal
            fftDataFilt = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nfft) ; // used to store the filtered FFT
            
            // Create Filter 0 and get its Fourier transform
            for (int i=0; i<filterLength; i++) {
                filterSamples[i] = 0;
            }
            filterSamples[1] = 1 ;
            filterSamples[100] = -0.8 ;
            filterSamples[1205] = 0.5 ;
            
            fftH0 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nfft) ;
            fft->processForward(filterSamples, fftH0, filterLength, nfft) ;
            
            // Create Filter 1 and get its Fourier transform
            for (int i=0; i<filterLength; i++) {
                filterSamples[i] = 0;
            }
            filterSamples[1] = 1 ;
            filterSamples[1670] = -0.8 ;
            filterSamples[340] = 0.5 ;
            fftH1 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nfft) ;
            fft->processForward(filterSamples, fftH1, filterLength, nfft) ;
            
            // Params init
            oldwet = wet ;
        }
        ~ConvolutionFilter() 
        {
            ftw_free(fftData) ;
            fftw_free(fftDataFilt) ;
            fftw_free(fftH0) ;
            fftw_free(fftH1) ;
            delete [] ifftSamples ;	
            delete [] filterSamples ;
            delete [] dryBuffer ;
            
            for (int k = 0; k < nbFilters; k++)
                delete [] olaBuffer[k];
            delete [] olaBuffer;           
        }

        void ProcessBlock(size_t numSamples, float ** input, float ** output) 
        {
            memset(output[0],0,numSamples*sizeof(float));
            memset(output[1],0,numSamples*sizeof(float));
            // Warning : buffer.getNumSamples can vary at each callback !
            for (int channel = 0; channel < numSamples; ++channel)
            {
                float* channelData = input[channel];
                int N = numOutputChannels;
                int bufsize = numSamples;
                
                
                // Fill up the dry circular Buffer, used to account for latency of the wet signals when summing dry and wet signals at the end
                for (int i = 0 ; i<dryBufferSize-bufsize ; i++)
                {
                    dryBuffer[i] = dryBuffer[i+bufsize] ;
                }
                for (int i = (dryBufferSize-bufsize) ; i<dryBufferSize ; i++)
                {
                    dryBuffer[i] = channelData[i-dryBufferSize+bufsize] ;
                }
                
                // Do the FFT
                fft->processForward(channelData, fftData, bufsize, nfft) ;
                
                // zeros the elements that have been previously outputted, put them at the end of the ola buffer
                for (int k=0; k<nbFilters; k++) {
                    for (int i = 0 ; i<nfft-bufsize ; i++)
                    {
                        olaBuffer[k][i] = olaBuffer[k][i+bufsize] ;
                    }
                    for (int i = (nfft-bufsize) ; i<nfft ; i++)
                    {
                        olaBuffer[k][i] = 0.0 ;
                    } 
                }
                
                // Multiply FFT with filter0
                for(int i = 0; i < nfft; i++) {
                    fftDataFilt[i][0] = fftData[i][0] * fftH0[i][0] - fftData[i][1] * fftH0[i][1] ;
                    fftDataFilt[i][1] = fftData[i][0] * fftH0[i][1] + fftData[i][1] * fftH0[i][0] ;
                }

                // Reverse FFT
                fft->processBackward(fftDataFilt, ifftSamples, nfft) ;
                                
                for (int i = 0 ; i<nfft ; i++) {
                    olaBuffer[0][i] += ifftSamples[i] ;
                }
                
                // Multiply FFT with filter1
                for(int i = 0; i < nfft; i++) {
                    fftDataFilt[i][0] = fftData[i][0] * fftH1[i][0] - fftData[i][1] * fftH1[i][1] ;
                    fftDataFilt[i][1] = fftData[i][0] * fftH1[i][1] + fftData[i][1] * fftH1[i][0] ;
                }
                
                // Reverse FFT
                fft->processBackward(fftDataFilt, ifftSamples, nfft) ;
                
                // Add to overlap-add (ola) buffer1
                for (int i = 0 ; i<nfft ; i++) {
                    olaBuffer[1][i] += ifftSamples[i] ;
                }
                
                for(size_t i = 0; i < bufsize; i++)
                {
                    output[0][i] += wet * olaBuffer[0][i] + (1.0-wet)*dryBuffer[0][i];
                    output[1][i] += wet * olaBuffer[0][i] + (1.0-wet)*dryBuffer[0][i];
                }
                // update parameters
                oldwet = wet ;
            }
        }            
    };


    FastFourierTransformer::FastFourierTransformer (int fftSize) {	
        data          = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fftSize);
        fft_result    = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fftSize);
        ifft_result   = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fftSize);
        
        plan_forward  = fftw_plan_dft_1d(fftSize, data, fft_result, FFTW_FORWARD, FFTW_MEASURE);
        plan_backward = fftw_plan_dft_1d(fftSize, data, ifft_result, FFTW_BACKWARD, FFTW_MEASURE);	
        
    }

    //-------------------------------------------------------------

    // destructor

    FastFourierTransformer::~FastFourierTransformer() {
        
        fftw_free(data);
        fftw_free(fft_result);
        fftw_free(ifft_result);
        fftw_destroy_plan(plan_forward);	
        fftw_destroy_plan(plan_backward);
        
    }

    //--------------------------------------------------------------

    // fft conversion method

    void FastFourierTransformer::processForward (float* channelData, fftw_complex* fftData, int dataSize, int fftSize) {
        
        for(i = 0; i < fftSize; i++) {
            
            if (i<dataSize){
                data[i][0] = channelData[i]; }      // stick your audio samples in here  
            else {
                data[i][0] = 0.0;					// zero padding for i>dataSize
            }
            data[i][1] = 0.0;                  // use this if your data is complex valued
        }
            
        fftw_execute(plan_forward);
        
        for (i = 0; i < fftSize; i++) {
            
            fftData[i][0] = fft_result[i][0] ;
            fftData[i][1] = fft_result[i][1] ;				
        }	
    }

    //--------------------------------------------------------------

    // inverse fft conversion method

    void FastFourierTransformer::processBackward (fftw_complex* fftData, float* ifftSamples, int fftSize) {
                
        for(i = 0; i < fftSize; i++) {
            
            data[i][0] = fftData[i][0];        // stick your fft data in here!
            data[i][1] = fftData[i][1];        // use this if your data is complex valued
        }	
        
        fftw_execute(plan_backward);
            
        for(i = 0; i < fftSize; i++) {
        
            ifftSamples[i] = ifft_result[i][0] / fftSize ;
            
            // fftw3 code is not normalized ! need to divide once in the fft/ifft process.
        }		
    }


    //--------------------------------------------------------------

    //  multiply two arrays of fft_complex

    fftw_complex* FastFourierTransformer::mult (fftw_complex* X, fftw_complex* Y, int fftSize) {
        for(i = 0; i < fftSize; i++) {
            data[i][0] = X[i][0] * Y[i][0] - X[i][1] * Y[i][1] ;
            data[i][1] = X[i][0] * Y[i][1] + X[i][1] * Y[i][0] ;
        }
        return data ;
    }

    /*
    //--------------------------------------------------------------

    //polar to cartesian conversion functions

    double FastFourierTransformer::real (double phase, double mag) { 
        
        return cos(phase) * mag;
    }



    double FastFourierTransformer::imag (double phase, double mag) {
        
        return sin(phase) * mag ;	
    }

    //--------------------------------------------------------------

    //cartesian to polar conversion functions

    double FastFourierTransformer::magnitude (double x, double y) {
        
        return sqrt(y * y + x * x);	
    }

    double FastFourierTransformer::angle (double x, double y)  { 
        
        if (x > 0) { return atan(y/x); }
        if (x < 0 && y >= 0) {return atan(y/x) + M_PI; }
        if (x < 0 && y < 0) {return atan(y/x) - M_PI; }
        if (y > 0) { return M_PI / 2; }
        if (y < 0) { return -M_PI / 2; }
        
        return 0;	
    }
    */