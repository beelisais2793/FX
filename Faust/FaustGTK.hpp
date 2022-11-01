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


struct FaustFX
{
    float ** input;
    float ** output;
    
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

    FaustFX(const char * file, size_t n = 256) {
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
        m_factory = createDSPFactoryFromString("faust", theCode, argc, argv, "", m_errorString, optimize);
        std::cout << m_errorString;
        // creating the DSP instance for interfacing
        m_dsp = m_factory->createDSPInstance();
        //interface = new GTKUI("faust",0,NULL);    
        //m_dsp->buildUserInterface(interface);
        // initializing the DSP instance with the SR
        m_dsp->init(44100);
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
        //delete m_ui;
        deleteDSPFactory(m_factory);
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
    
};


struct FaustPolyFX
{
    float ** input;
    float ** output;
    //GTKUI* interface;
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

    FaustPolyFX(const char * file, size_t n = 256) {
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
        //interface = new GTKUI("faust",0,NULL);    
        //m_dsp->buildUserInterface(interface);
        // initializing the DSP instance with the SR
        m_dsp->init(44100);
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
        //delete m_ui;
        delete m_factory;
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
    void noteOn(int pitch, int velocity)
    {
        m_dsp->keyOn(0,pitch,velocity);
    }
    void noteOff(int pitch, int velocity)
    {
        m_dsp->keyOff(0,pitch,velocity);
    }
};

//std::list<GUI*> GUI::fGuiList;
//ztimedmap GUI::gTimedZoneMap;

