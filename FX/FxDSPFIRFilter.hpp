#pragma once

namespace DSPFX
{

    struct FxFIRFilter : public MonoFXProcessor
    {
        /** Convolution Algorithm to use */
        enum 
        {
            /** Choose the best algorithm based on filter size */
            BEST    = 0,

            /** Use direct convolution */
            DIRECT  = 1,

            /** Use FFT Convolution (Better for longer filter kernels */
            FFT     = 2

        };

        FIRFilter * fir;

        FxFIRFilter(DspFloatType * kernel, size_t len, int mode) : MonoFXProcessor() {
            fir = FIRFilterInitD(kernel,len,mode);
            assert(fir != NULL);
        }
        ~FxFIRFilter() {
            if(fir) FIRFilterFreeD(fir);
        }
        void flush() { FIRFilterFlushD(fir); }

        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            FIRFilterProcessD(fir,out,in,n);
        }
        void updateKernel(DspFloatType * kernel) {
            FIRFilterUpdateKernelD(fir,kernel);
        }
        enum {
            PORT_KERNEL,
        };
        void setPortV(int port, const std::vector<DspFloatType> & k) {
            if(port == PORT_KERNEL) updateKernel(k.data());
            else printf("No port %d",port);
        }
    };
}
