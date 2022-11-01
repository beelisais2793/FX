#pragma once 

namespace DSP 
{
        using dft_order = kfr::dft_order;

    template<typename T>
    struct DFTPlan 
    {
    private:
        kfr::dft_plan<T> *plan;
    public:
        DFTPlan(size_t size) {
            plan = new kfr::dft_plan<T>(size);
        }
        ~DFTPlan() {
            if(plan) delete plan;
        }
        void dump() const { plan->dump(); }
        
        void execute(kfr::univector<kfr::complex<T>> & out, const kfr::univector<kfr::complex<T>> & in, bool inverse = false) {
            kfr::univector<kfr::u8> temp;
            temp.resize(in.size());
            out.resize(in.size());
            plan->execute(out.data(),in.data(),temp.data(),inverse);
        }
    };

    template<typename T>
    struct DFTRealPlan
    {
    private:
        kfr::dft_plan_real<T> *plan;
    public:
        DFTRealPlan(size_t size) {
            plan = new kfr::dft_plan_real<T>(size);
        }
        ~DFTRealPlan() {
            if(plan) delete plan;
        }
        void dump() const { plan->dump(); }

        void execute_forward(kfr::univector<kfr::complex<T>> & out, const kfr::univector<T> & in) {
            kfr::univector<kfr::u8> temp;
            temp.resize(in.size());
            out.resize(in.size());
            plan->execute(out,in,temp);
        }
        void execute_reverse(kfr::univector<T> & out, const kfr::univector<kfr::complex<T>> & in) {
            kfr::univector<kfr::u8> temp;
            temp.resize(in.size());
            out.resize(in.size());
            plan->execute(out,in,temp);
        }
    };

    template<typename T>
    kfr::univector<kfr::complex<T>> run_dft(const kfr::univector<kfr::complex<T>> & data) { 
        return kfr::univector<kfr::complex<T>>(kfr::dft(data)); 
    }

    // sets imaginary to zero, return complex
    template<typename T>
    kfr::univector<kfr::complex<T>> run_realdft(const kfr::univector<T> & data) { 
        kfr::univector<kfr::complex<T>> tmp;
        tmp.resize(data.size());    
        for(size_t i = 0; i < data.size(); i++)
        {
            tmp[i] = kfr::complex<T>(data[i],(T)0);
        }
        return kfr::univector<kfr::complex<T>>(kfr::dft(tmp)); 
    }

    template<typename T>
    kfr::univector<kfr::complex<T>> run_idft(const kfr::univector<kfr::complex<T>> & data) { return kfr::univector<kfr::complex<T>>(kfr::idft(data)); }

    template<typename T>
    kfr::univector<T> run_irealdft(const kfr::univector<kfr::complex<T>> & data) { return kfr::univector<T>(kfr::irealdft(data)); }

}