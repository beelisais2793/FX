#pragma once

#include "StkHeaders.hpp"

namespace Stk 
{
    struct BandedWG : public GeneratorProcessorPlugin<stk::BandedWG>
    {
        BandedWG() : GeneratorProcessorPlugin<stk::BandedWG>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,            
            PORT_STRIKEPOS,
            PORT_PRESET,
            PORT_STARTBOW,
            PORT_STOPBOW,
            PORT_PLUCK,            
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_PRESET: this->setPreset(v); break;
                case PORT_STOPBOW: this->stopBowing(v); break;
                case PORT_PLUCK: this->pluck(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                case PORT_STARTBOW: this->startBowing(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };

    struct BlowBotl : public GeneratorProcessorPlugin<stk::BlowBotl>
    {
        BlowBotl() : GeneratorProcessorPlugin<stk::BlowBotl>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,  
            PORT_STARTBLOW,
            PORT_STOPBLOW,                      
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_STOPBLOW: this->stopBlowing(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                case PORT_STARTBLOW: this->startBlowing(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };
    struct BlowHole : public GeneratorProcessorPlugin<stk::BlowHole>
    {
        BlowHole() : GeneratorProcessorPlugin<stk::BlowHole>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,            
            PORT_TONEHOLE,
            PORT_VENT,
            PORT_STARTBLOW,
            PORT_STOPBLOW,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_TONEHOLE: this->setTonehole(v); break;
                case PORT_VENT: this->setVent(v); break;
                case PORT_STOPBLOW: this->stopBlowing(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                case PORT_STARTBLOW: this->startBlowing(v); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };    
    struct BowTable : public GeneratorProcessorPlugin<stk::BowTable>
    {
        BowTable() : GeneratorProcessorPlugin<stk::BowTable>()
        {

        }enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,  
            PORT_OFFSET,
            PORT_SLOPE,
            PORT_MIN,
            PORT_MAX,          
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_OFFSET: this->setOffset(v); break;
                case PORT_SLOPE: this->setSlope(v); break;
                case PORT_MIN: this->setMinOutput(v); break;
                case PORT_MAX: this->setMaxOutput(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                default: printf("No port %d\n",port);
            }        
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };
    struct Bowed : public GeneratorProcessorPlugin<stk::Bowed>
    {
        Bowed() : GeneratorProcessorPlugin<stk::Bowed>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,     
            PORT_VIBRATO,
            PORT_STARTBOW,
            PORT_STOPBOW,       
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_VIBRATO: this->setVibrato(v); break;
                case PORT_STOPBOW: this->stopBowing(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                case PORT_STARTBOW: this->startBowing(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };
    struct Brass : public GeneratorProcessorPlugin<stk::Brass>
    {
        Brass() : GeneratorProcessorPlugin<stk::Brass>()
        {

        }
         enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,     
            PORT_LIP,
            PORT_STARTBLOW,
            PORT_STOPBLOW,            
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_VIBRATO: this->setVibrato(v); break;
                case PORT_LIP: this->setLip(v); break;
                case PORT_STOPBLOW: this->stopBlowing(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                case PORT_STARTBLOW: this->startBlowing(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };

    struct Clarinet : public GeneratorProcessorPlugin<stk::Clarinet>
    {
        Clarinet() : GeneratorProcessorPlugin<stk::Clarinet>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,            
            PORT_STARTBLOW,
            PORT_STOPBLOW,            
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_STOPBLOW: this->stopBlowing(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                case PORT_STARTBLOW: this->startBlowing(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };

        // todo: ports
    struct Flute : public GeneratorProcessorPlugin<stk::Flute>
    {
        Flute() : GeneratorProcessorPlugin<stk::Flute>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,            
            PORT_STARTBLOW,
            PORT_STOPBLOW, 
            PORT_JETREFLECT,
            PORT_ENDREFLECT,
            PORT_JETDELAY,           
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_STOPBLOW: this->stopBlowing(v); break;
                case PORT_JETREFLECT: this->setJetReflection(v); break;
                case PORT_ENDREFLECT: this->setEndReflection(v); break;
                case PORT_JETDELAY: this->setJetDelay(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                case PORT_STARTBLOW: this->startBlowing(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };    

    // todo: ports
    struct Guitar : public GeneratorProcessorPlugin<stk::Guitar>
    {
        Guitar() : GeneratorProcessorPlugin<stk::Guitar>()
        {

        }
        Guitar(const std::string & bodyfile) : GeneratorProcessorPlugin<stk::Guitar>()
        {
            this->setBodyFile(bodyfile);
        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,            
            PORT_PLUCKPOS,
            PORT_LOOPGAIN,            
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_PLUCK: this->setPluckPosition(v); break;
                case PORT_LOOPGAIN: this->setLoopGain(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                case PORT_PLUCK: this->setPluckPosition(a,b); break;
                case PORT_LOOPGAIN: this->setLoopGain(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };

    // todo: ports
    struct JetTable : public FunctionProcessorPlugin<stk::JetTable>
    {
        JetTable() : FunctionProcessorPlugin<stk::JetTable>()
        {

        }        
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };

    // todo: ports
    struct Mandolin : public GeneratorProcessorPlugin<stk::Mandolin>
    {
        Mandolin() : GeneratorProcessorPlugin<stk::Mandolin>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,  
            PORT_DETUNE,
            PORT_BODYSIZE,
            PORT_PLUCK,                      
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_DETUNE: this->setDetune(v); break;
                case PORT_BODYSIZE: this->setBodySize(v); break;
                case PORT_PLUCK: this->pluck(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                case PORT_PLUCK: this->pluck(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };

    // todo: ports
    struct Mesh2D : public GeneratorProcessorPlugin<stk::Mesh2D>
    {
        Mesh2D() : GeneratorProcessorPlugin<stk::Mesh2D>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,  
            PORT_NX,
            PORT_NY,
            PORT_INPUTPOS,
            PORT_DECAY,            
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_NX: this->setNX(v); break;
                case PORT_NY: this->setNY(v); break;
                case PORT_DECAY: this->setDecay(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                case PORT_INPUTPOS: this->setInputPosition(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };

    // todo: ports
    struct Modal : public GeneratorProcessorPlugin<stk::Modal>
    {
        Modal() : GeneratorProcessorPlugin<stk::Modal>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,            
            PORT_CLEAR,            
            PORT_RATIORADIUS,
            PORT_MASTERGAIN,
            PORT_DIRECTGAIN,
            PORT_MODEGAIN,
            PORT_STRIKE,
            PORT_DAMP,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_MASTERGAIN: this->setMasterGain(v); break;
                case PORT_DIRECTGAIN: this->setDirectGain(v); break;
                case PORT_MODEGAIN: this->setModeGain(v); break;
                case PORT_STRIKE: this->strike(v); break;
                case PORT_DAMP: this->damp(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                case PORT_RATIORADIUS: this->setRatioAndRadius(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };

    // todo: ports
    struct Plucked : public GeneratorProcessorPlugin<stk::Plucked>
    {
        Plucked() : GeneratorProcessorPlugin<stk::Plucked>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,  
            PORT_PLUCK,     
            PORT_STARTBLOW,
            PORT_STOPBLOW,      
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_PLUCK: this->pluck(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };

    // todo: ports
    struct Recorder : public GeneratorProcessorPlugin<stk::Recorder>
    {
        Recorder() : GeneratorProcessorPlugin<stk::Recorder>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,   
            PORT_STARTBLOW,
            PORT_STOPBLOW,         
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_STOPBLOW: this->stopBlowing(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                case PORT_STARTBLOW: this->startBlowing(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };

    // todo: ports
    struct ReedTable : public FunctionProcessorPlugin<stk::ReedTable>
    {
        ReedTable() : FunctionProcessorPlugin<stk::ReedTable>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,            
            PORT_OFFSET,
            PORT_SLOPE
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_OFFSET: this->setOffset(v); break;
                case PORT_SLOPE: this->setSlope(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };
    
        // todo: ports
    struct Saxophony : public GeneratorProcessorPlugin<stk::Saxophony>
    {
        Saxophony() : GeneratorProcessorPlugin<stk::Saxophony>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,            
            PORT_STARTBLOW,
            PORT_STOPBLOW,         
            PORT_BLOWPOS,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_STOPBLOW: this->stopBlowing(v); break;
                case PORT_BLOWPOS: this->setBlowPosition(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                case PORT_STARTBLOW: this->startBlowing(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };

    // todo: ports
    struct Shakers : public GeneratorProcessorPlugin<stk::Shakers>
    {
        Shakers() : GeneratorProcessorPlugin<stk::Shakers>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,            
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };

    // todo: ports
    struct Sitar : public GeneratorProcessorPlugin<stk::Sitar>
    {
        Sitar() : GeneratorProcessorPlugin<stk::Sitar>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR, 
            PORT_PLUCK,           
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_PLUCK: this->pluck(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };

    // todo: ports
    struct StifKarp : public GeneratorProcessorPlugin<stk::StifKarp>
    {
        StifKarp() : GeneratorProcessorPlugin<stk::StifKarp>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,  
            PORT_STRETCH,
            PORT_PICKUPOS,
            PORT_BASELOOPGAIN,          
            PORT_PLUCK,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_STRETCH: this->setStretch(v); break;
                case PORT_PICKUPOS: this->setPickupPosition(v); break;
                case PORT_BASELOOPGAIN: this->setBaseLoopGain(v); break;
                case PORT_PLUCK: this->pluck(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };

    // todo: ports
    struct Twang : public FunctionProcessorPlugin<stk::Twang>
    {
        Twang() : FunctionProcessorPlugin<stk::Twang>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,   
            PORT_LOWFREQ,
            PORT_PLUCKPOS,
            PORT_LOOPGAIN,
            PORT_LOOPFILTER,         
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_LOWFREQ: this->setLowestFrequency(v); break;
                case PORT_PLUCKPOS: this->setPluckPosition(v); break;
                case PORT_LOOPGAIN: this->setLoopGain(v); break;                
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                default: printf("No port %d\n",port);
            }
        }        
        void setPortV(int port, const std::vector<double> & v) {
            if(port == PORT_LOOPFILTER) this->setLoopFilter(v);
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };

    
    struct Whistle : public GeneratorProcessorPlugin<stk::Whistle>
    {
        Whistle() : GeneratorProcessorPlugin<stk::Whistle>()
        {

        }
        enum {
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_FREQ,
            PORT_CC,
            PORT_CLEAR,            
            PORT_STARTBLOW,
            PORT_STOPBLOW,         
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_NOTEOFF: this->noteOff(v); break;
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_CLEAR: this->clear(); break;
                case PORT_STOPBLOW: this->stopBlowing(v); break;
                default: printf("No port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->noteOn(a,b); break;
                case PORT_CC: this->controlChange(a,b); break;
                case PORT_STARTBLOW: this->startBlowing(a,b); break;
                default: printf("No port %d\n",port);
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };
}