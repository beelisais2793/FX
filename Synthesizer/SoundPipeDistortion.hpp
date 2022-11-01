#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct Distortion : public SoundPiper
    {    
        Distortion(sp_data * data) :
        SoundPiper(data,(create_func)sp_dist_create,(init_func)sp_dist_init,(create_func)sp_dist_destroy,(compute_func)sp_dist_compute) 
        {        
        
        }        
        void setPreGain(float pre) {
            sp_dist * p = (sp_dist*)ptr;
            p->pregain = pre;
        }
        void setPostGain(float post) {
            sp_dist * p = (sp_dist*)ptr;
            p->postgain = post;
        }
        void setShape1(float shape) {
            sp_dist * p = (sp_dist*)ptr;
            p->shape1 = shape;
        }
        void setShape2(float shape) {
            sp_dist * p = (sp_dist*)ptr;
            p->shape2 = shape;
        }
    };    
}