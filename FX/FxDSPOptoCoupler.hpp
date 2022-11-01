#pragma once

namespace DSPFX
{
    struct FxOptoCoupler
    {
        enum 
        {
            /** Light-Dependent-Resistor output. Based
             on Vactrol VTL series. datasheet:
            http://pdf.datasheetcatalog.com/datasheet/perkinelmer/VT500.pdf
            Midpoint Delay values:
            Turn-on delay:   ~10ms
            Turn-off delay:  ~133ms
            */
            OPTO_LDR,

            /** TODO: Add Phototransistor/voltage output opto model*/
            OPTO_PHOTOTRANSISTOR
        };

        OptoCoupler * oc;
        double delay;

        FxOptoCoupler(int type, DspFloatType delay, DspFloatType sr=sampleRate)
        {
            this->delay = delay;
            oc = OptoInitD(type,delay,sr);
            assert(oc != NULL);
        }
        ~FxOptoCoupler() {
            OptoFreeD(oc);
        }
        void setDelay(DspFloatType d) {
            delay = d;
            OptoSetDelayD(oc,d);
        }
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out)
        {
            OptoProcessD(oc,out,in,n);
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            double d = delay;
            setDelay(d*X*Y);
            double r = OptoTickD(I);
            setDelay(d);
            return A*r;
        }
        enum {
            PORT_DELAY,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_DELAY: setDelay(v); break;
                default: printf("no port %d\n",port);
            }
        }
    };
}
