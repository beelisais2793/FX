%{
#include "Gamma/HRFilter.h"
%}

namespace gam {

    class HRFilter
    {
    public:

        HRFilter(){}

        gam::Dist<3>& dist();        
        HRFilter& earDist(float v);

        template <class Vec3, class Mat4>
        HRFilter& pos(const Vec3& sourcePos, const Mat4& headPose);

        %extend {
            gam::float3 process(float src) { return (*$self)(src); }
        }

    };


    
    template <int Nsrc>
    class HRScene{
    public:
        typedef HRFilter Source;

        HRScene();
        
        int numSources() const;
        int size() const;

        
        Source& source(int i);
        
        float& sample(int i);
        const float& sample(int i) const;
        HRScene& zeroSamples();
        HRScene& clear();

        
        HRScene& active(int i, bool v);
        bool active(int i) const;

        
        %extend {
            gam::float2 bang() { return (*$self)(); }
        }
        
        HRScene& far(float v);
        HRScene& reverbDecay(float v);
        HRScene& reverbDamping(float v);
        HRScene& wallAtten(float v);
    };
}