#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Distortion
{

    struct DiodeClipper : public ATKFilter
    {
        ATK::DiodeClipperFilter<DspFloatType> * filter;

        DiodeClipper()
        {
            filter = new ATK::DiodeClipperFilter<DspFloatType>;
            this->setFilter(filter);
        }
        ~DiodeClipper() {
            if(filter) delete filter;
        }
    };

    struct HalfTanhShaper : public ATKFilter
    {
        ATK::HalfTanhShaperFilter<DspFloatType> * filter;

        HalfTanhShaper()
        {
            filter = new ATK::HalfTanhShaperFilter<DspFloatType>(2);
            this->setFilter(filter);
        }
        ~HalfTanhShaper() {
            if(filter) delete filter;
        }
        enum {
            PORT_COEFF,
        };

        void setPort(int port, DspFloatType value)
        {
            switch(port)
            {
                case PORT_COEFF: filter->set_coefficient(value);
            }
        }
    };
    struct SD1Overdrive : public ATKFilter
    {
        ATK::SF1OverdriveFilter<DspFloatType> * filter;

        SD1Overdrive()
        {
            filter = new ATK::SF1OverdriveFilter<DspFloatType>;
            this->setFilter(filter);
        }
        ~SD1Overdrive() {
            if(filter) delete filter;
        }
        enum
        {
            PORT_DRIVE
        };
        void setPort(int port, DspFloatType value) {
            swtich(port) {
                case PORT_DRIVE: filter->set_drive(value);
            }
        }
    };
    struct TS9Overdrive : public ATKFilter
    {
        ATK::TS9OverdriveFilter<DspFloatType> * filter;
        TS9Overdrive()
        {
            filter = new ATK::TS9OverdriveFilter<DspFloatType>;
            this->setFilter(filter);
        }
        ~TS9Overdrive() {
            if(filter) delete filter;
        }
        enum
        {
            PORT_DRIVE
        };
        void setPort(int port, DspFloatType value) {
            swtich(port) {
                case PORT_DRIVE: filter->set_drive(value);
            }
        }
    };
    struct TanhShaper : public ATKFilter
    {
        ATK::TanhShaperFilter<DspFloatType> * filter;
        TanhShaper()
        {
            filter = new ATK::TanhShaperFilter<DspFloatType>(2);
            this->setFilter(filter);
        }
        ~TanhShaper() {
            if(filter) delete filter;
        }
        enum {
            PORT_COEFF,
        };

        void setPort(int port, DspFloatType value)
        {
            switch(port)
            {
                case PORT_COEFF: filter->set_coefficient(value);
            }
        }
    };
}