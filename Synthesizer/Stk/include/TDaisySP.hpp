
#include "daisysp.h"

union DaisySP_Object
{
    daisysp::AdEnv * adenv;
}

struct DaisySP
{
    DaisySP_Object object;

    enum ModuleType
    {
        ADENV,
    };

    DaisySP(ModuleType type) {
        switch(type)
        {
            case ADENV: object.adenv = new daisysp::AdEnv; break;
        }
    }

    daisysp::AdEnv* GetADEnv() const { return object.adenv; 

};