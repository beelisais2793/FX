#pragma once 

namespace DSP 
{
        template<typename T>
    kfr::univector<T> convolve(const kfr::univector<T> & v1, const kfr::univector<T> & v2) {
        return kfr::univector<T>(kfr::convolve(v1,v2));
    }

    template<typename T>
    struct ConvolveFilter {
    private:
        kfr::convolve_filter<T> *filter;
    
    public:
        ConvolveFilter(size_t size, size_t block_size=1024) {
            filter = new kfr::convolve_filter<T>(size,block_size);
            assert(filter != nullptr);
        }
        ConvolveFilter(kfr::univector<T> & u, size_t block_size=1024) {
            filter = new kfr::convolve_filter<T>(u,block_size);
            assert(filter != nullptr);
        }
        ~ConvolveFilter() {
            if(filter) delete filter;
        }
        void set_data(const kfr::univector<T> & u) { filter->set_data(u); }
        void reset() { filter->reset(); }
        size_t input_block_size() const { return filter->input_block_size(); }

        void apply(kfr::univector<T> & in) {
            filter->apply(in);
        }
        void apply(const kfr::univector<T> & in, kfr::univector<T> & out) {
            filter->apply(out,in);
        }
    };  
}