#pragma once 

namespace DSP
{
    template<typename T>
    struct DCTPlan 
    {
    private:    
        kfr::dct_plan<T> plan;    
    public:
        DCTPlan(size_t size) : plan(size) {}
        
        void execute(kfr::univector<T> & out, const kfr::univector<T> & in, bool inverse=false) {
            kfr::univector<kfr::u8> temp(in.size());
            plan.execute(out.data(),in.data(),temp.data(),inverse);
        }
    };
}