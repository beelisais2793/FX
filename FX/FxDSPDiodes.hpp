namespace DSPFX
{

    struct FxDiodeRectifier : public MonoFXProcessor
    {
        enum _bias_t
        {
        /** Pass positive signals, clamp netagive signals to 0 */
        FORWARD_BIAS,

        /** Pass negative signals, clamp positive signals to 0 */
        REVERSE_BIAS,

        /** Full-wave rectification. */
        FULL_WAVE
        };
        DiodeRectifier * rect;

        FxDiodeRectifier( _bias_t bias, DspFloatType thresh) : MonoFXProcessor()
        {
            rect = DiodeRectifierInitD((bias_t)bias,thresh);
            assert(rect != NULL);
        }
        ~FxDiodeRectifier() {
            if(rect) DiodeRectifierFreeD(rect);
        }
        void setThreshold(DspFloatType t) {
            Error_t err = DiodeRectifierSetThresholdD(rect,t);
        }
        enum {
            PORT_THRESHOLD,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_THRESHOLD: setThreshold(v); break;
                default: printf("No Port %d\n",port);
            }
        }
        void ProcessBlock(size_t n, const DspFloatType * in, DspFloatType * out) {
            Error_t err = DiodeRectifierProcessD(rect,out,in,n);
        }
        DspFloatType Tick(DspFloatType in) {
            return DiodeRectifierTickD(rect,in);
        }
    };

    struct FxDiodeSaturator : public MonoFXProcessor
    {
        enum _bias_t
        {
        /** Pass positive signals, clamp netagive signals to 0 */
        FORWARD_BIAS,

        /** Pass negative signals, clamp positive signals to 0 */
        REVERSE_BIAS,

        /** Full-wave rectification. */
        FULL_WAVE
        };
        DiodeSaturator * rect;

        FxDiodeSaturator( _bias_t bias, DspFloatType amt) : MonoFXProcessor()
        {
            rect = DiodeSaturatorInitD((dbias_t)bias,amt);
            assert(rect != NULL);
        }
        ~FxDiodeSaturator() {
            if(rect) DiodeSaturatorFreeD(rect);
        }
        void setThreshold(DspFloatType t) {
            Error_t err = DiodeSaturatorSetAmountD(rect,t);
        }
        enum {
            PORT_THRESHOLD,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_THRESHOLD: setThresholdD(v); break;
                default: printf("No Port %d\n",port);
            }
        }
        void ProcessBlock(size_t n, const DspFloatType * in, DspFloatType * out) {
            Error_t err = DiodeSaturatorProcessD(rect,out,in,n);
        }
        DspFloatType Tick(DspFloatType in, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            return DiodeSaturatorTickD(rect,in);
        }        
    };
}
