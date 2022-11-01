#pragma once

// faust
#include "faust/dsp/llvm-dsp.h"
#include "faust/dsp/libfaust.h"
#include "faust/audio/dummy-audio.h"
#include "faust/gui/DecoratorUI.h"
#include "faust/gui/PrintUI.h"
#include "faust/misc.h"
#include "faust/dsp/proxy-dsp.h"
#include "faust/dsp/poly-llvm-dsp.h"


namespace FX
{
    struct SliderKaka
    {
        FAUSTFLOAT *zone;
        FAUSTFLOAT fInit;
        FAUSTFLOAT fMin;
        FAUSTFLOAT fMax;
        FAUSTFLOAT fStep;    

        SliderKaka() = default;

        SliderKaka(const SliderKaka & kaka) {
            zone = kaka.zone;
            fInit = kaka.fInit;
            fMin = kaka.fMin;
            fMax = kaka.fMax;
            fStep = kaka.fStep;        
        }
        SliderKaka& operator = (const SliderKaka & kaka)
        {
            zone = kaka.zone;
            fInit = kaka.fInit;
            fMin = kaka.fMin;
            fMax = kaka.fMax;
            fStep = kaka.fStep;        
            return *this;
        }
    };

    struct DummyUI : public GenericUI 
    {    
        std::map<std::string,SliderKaka> controls;
        std::vector<std::string> names;

        DummyUI()
        {
            
        }
        /*
        // -- widget's layouts
        virtual void openTabBox(const char* label) {}
        virtual void openHorizontalBox(const char* label) {}
        virtual void openVerticalBox(const char* label) {}
        virtual void closeBox() {}
        
        // -- active widgets
        virtual void addButton(const char* label, FAUSTFLOAT* zone) {}
        virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) {}
        virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) {}
        virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) {}
        virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) {}

        // -- passive widgets
        virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {}
        virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {}

        // -- soundfiles
        virtual void addSoundfile(const char* label, const char* soundpath, Soundfile** sf_zone) {}

        virtual void declare(FAUSTFLOAT* zone, const char* key, const char* val) {}
        
        */
    /*
        void declare(FAUSTFLOAT* zone, const char* key, const char* val)
        {
            printf("%s, %s\n",key,val);
        }
        */
        void addButton(const char* label, FAUSTFLOAT* zone)
        {
            SliderKaka kaka;
            kaka.zone = zone;
            kaka.fInit = 0;
            kaka.fMin  = 0;
            kaka.fMax = 1;
            kaka.fStep = 1;        
            controls[label] = kaka;
        }
        void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            SliderKaka kaka;
            kaka.zone = zone;
            kaka.fInit = init;
            kaka.fMin  = min;
            kaka.fMax = max;
            kaka.fStep = step;        
            controls[label] = kaka;
        }
        void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            SliderKaka kaka;
            kaka.zone = zone;
            kaka.fInit = init;
            kaka.fMin  = min;
            kaka.fMax = max;
            kaka.fStep = step;        
            controls[label] = kaka;
            names.push_back(label);
        }
        void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            SliderKaka kaka;
            kaka.zone = zone;
            kaka.fInit = init;
            kaka.fMin  = min;
            kaka.fMax = max;
            kaka.fStep = step;        
            controls[label] = kaka;
            names.push_back(label);
        }
        void setControl(const std::string& name, float value)
        {
            if( controls.find(name) == controls.end()) return;          
            if(value < controls[name].fMin ) value = controls[name].fMin;
            if(value > controls[name].fMax ) value = controls[name].fMax;        
            *controls[name].zone = value;               
        }
        float getControl(const std::string& name) {
            if( controls.find(name) == controls.end()) return -99999999999;
            return *controls[name].zone;
        }
    };


    struct FaustFX : public StereoFXProcessor
    {
        float ** input;
        float ** output;
        DummyUI * interface;
        llvm_dsp_factory* m_factory;
        dsp* m_dsp;

        std::string read_file(const char * filename)
        {
            FILE * f = fopen(filename,"r");
            std::string r;
            char buffer[1024];
            char * c;
            while((c = fgets(buffer,1024,f)) != NULL) {    
                r += c;
            }
            return r;
        }

        FaustFX(const char * file, size_t n = 256) 
        : StereoFXProcessor()
        {
            input = (float**)calloc(2,sizeof(float*));
            output = (float**)calloc(2,sizeof(float*));
            for(size_t i = 0; i < 2; i++)
            {
                input[i] = (float*)calloc(n,sizeof(float*));            
                output[i] = (float*)calloc(n,sizeof(float*));
            }

            // the Faust code to compile as a string (could be in a file too)
            std::string theCode = read_file(file);
            //std::cout << theCode << std::endl;
            // compiling in memory (createDSPFactoryFromFile could be used alternatively)
            int argc=0;
            const char *argv[] = {NULL};
            std::string m_errorString;
            bool optimize=true;
            m_factory = createDSPFactoryFromString("faust", theCode, 0, NULL, "", m_errorString, optimize);
            std::cout << m_errorString;
            // creating the DSP instance for interfacing
            m_dsp = m_factory->createDSPInstance();
            interface = new DummyUI();
            m_dsp->buildUserInterface(interface);
            
            // initializing the DSP instance with the SR
            m_dsp->init(44100);
            m_dsp->instanceResetUserInterface();
            //Thread gui(gui_thread,interface);
        }
        ~FaustFX()
        {        
            for(size_t i = 0; i < 2; i++)
            {
                free(input[i]);
                free(output[i]);
            }
            free(input);
            free(output);        
            delete m_dsp;        
            deleteDSPFactory(m_factory);
            if(interface) delete interface;
        }
        void Run(size_t framesPerBuffer, float ** inputs, float ** outputs)
        {        
            m_dsp->compute(framesPerBuffer, inputs, outputs);
        }
        void Run(size_t framesPerBuffer, float * in, float * out)
        {
            memcpy(input[0],in,framesPerBuffer*sizeof(float));
            memcpy(input[1],in,framesPerBuffer*sizeof(float));
            m_dsp->compute(framesPerBuffer, input, output);
            memcpy(out,output[0],framesPerBuffer*(sizeof(float)));
        }
        void Run2x1(size_t framesPerBuffer, float ** in, float * out)
        {        
            m_dsp->compute(framesPerBuffer, in, output);
            memcpy(out,output[0],framesPerBuffer*(sizeof(float)));
        }
        void Run1x2(size_t framesPerBuffer, float * in, float ** out)
        {
            memcpy(input[0],in,framesPerBuffer*sizeof(float));
            memcpy(input[1],in,framesPerBuffer*sizeof(float));
            m_dsp->compute(framesPerBuffer, input, out);        
        }
        void setControl(const std::string & name, float value) {
            interface->setControl(name,value);
        }
        float getControl(const std::string & name) {
            return interface->getControl(name);
        }

        void ProcessBlock(size_t n, float ** input, float ** output) {
            Run(n,input,output);
        }
    };


    struct FaustPolyFX : public StereoFXProcessor
    {
        float ** input;
        float ** output;
        DummyUI * interface;
        llvm_dsp_poly_factory* m_factory;
        dsp_poly* m_dsp;


        std::string read_file(const char * filename)
        {
            FILE * f = fopen(filename,"r");
            std::string r;
            char buffer[1024];
            char * c;
            while((c = fgets(buffer,1024,f)) != NULL) {    
                r += c;
            }
            return r;
        }

        FaustPolyFX(const char * file, size_t n = 256) 
        : StereoFXProcessor()
        {
            input = (float**)calloc(2,sizeof(float*));
            output = (float**)calloc(2,sizeof(float*));
            for(size_t i = 0; i < 2; i++)
            {
                input[i] = (float*)calloc(n,sizeof(float*));            
                output[i] = (float*)calloc(n,sizeof(float*));
            }

            // the Faust code to compile as a string (could be in a file too)
            std::string theCode = read_file(file);
            //std::cout << theCode << std::endl;
            // compiling in memory (createDSPFactoryFromFile could be used alternatively)
            int argc=0;
            const char *argv[] = {NULL};
            std::string m_errorString;
            bool optimize=true;
            m_factory = createPolyDSPFactoryFromString("faust", theCode, argc, argv, "", m_errorString, optimize);
            std::cout << m_errorString;
            
            m_dsp = m_factory->createPolyDSPInstance(8,true,true,false);
            interface = new DummyUI();        
            m_dsp->buildUserInterface(interface);
            
            // initializing the DSP instance with the SR
            m_dsp->init(44100);
            m_dsp->instanceResetUserInterface();
            //Thread gui(gui_thread,interface);
        }
        ~FaustPolyFX()
        {        
            for(size_t i = 0; i < 2; i++)
            {
                free(input[i]);
                free(output[i]);
            }
            free(input);
            free(output);        
            delete m_dsp;        
            delete m_factory;
            delete interface;
        }
        void Run(size_t framesPerBuffer, float ** inputs, float ** outputs)
        {        
            GUI::updateAllGuis();
            m_dsp->compute(framesPerBuffer, inputs, outputs);
        }
        void Run(size_t framesPerBuffer, float * in, float * out)
        {
            GUI::updateAllGuis();
            memcpy(input[0],in,framesPerBuffer*sizeof(float));
            memcpy(input[1],in,framesPerBuffer*sizeof(float));
            m_dsp->compute(framesPerBuffer, input, output);
            memcpy(out,output[0],framesPerBuffer*(sizeof(float)));
        }
        void Run2x1(size_t framesPerBuffer, float ** in, float * out)
        {        
            GUI::updateAllGuis();
            m_dsp->compute(framesPerBuffer, in, output);
            memcpy(out,output[0],framesPerBuffer*(sizeof(float)));
        }
        void Run1x2(size_t framesPerBuffer, float * in, float ** out)
        {
            GUI::updateAllGuis();
            memcpy(input[0],in,framesPerBuffer*sizeof(float));
            memcpy(input[1],in,framesPerBuffer*sizeof(float));
            m_dsp->compute(framesPerBuffer, input, out);        
        }
        void noteOn(int pitch, int velocity)
        {        
            m_dsp->keyOn(0,pitch,velocity);
        }
        void noteOff(int pitch, int velocity)
        {
            m_dsp->keyOff(0,pitch,velocity);
        }
        void setControl(const std::string & name, float value) {
            interface->setControl(name,value);
        }
        float getControl(const std::string & name) {
            return interface->getControl(name);
        }

        void ProcessBlock(size_t n, float ** input, float ** output) {
            Run(n,input,output);
        }
    };

    std::list<GUI*> GUI::fGuiList;
    ztimedmap GUI::gTimedZoneMap;

}