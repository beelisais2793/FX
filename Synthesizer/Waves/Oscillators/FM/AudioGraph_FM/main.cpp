//
//  main.cpp
//  AudioGraph_FM
//
//  Created by Owen Campbell on 2/19/15.
//  Copyright (c) 2015 Owen Campbell. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"
#include "sndfile.h"
#include <iostream>
#include <memory>
#include <limits>
#include <math.h>
#include <assert.h>
#include "Ugens.h"

#define CIRCUIT 3

SNDFILE * sf;
const int BLOCK_SIZE = 128;
const int sr = 44100;
const float dur_sec = 5;
const int sfSamples = dur_sec * sr;
float sfBuffer[sfSamples];
float * writePos = sfBuffer;

typedef struct
{
    root_ptr_f tree;
    int numOuts;
} paData;

static int paCallback( const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData )
{
    /* Cast data passed through stream to our structure. */
    paData *data = (paData*)userData;
    float *out = (float*)outputBuffer;
    float *in = (float*)inputBuffer;
    unsigned int i, j;
    float temp = 0;
    (void) in; /* Prevent unused variable warning. */
    for(i = 0; i < framesPerBuffer; i++){
        data->tree->traverse();
        for(j = 0; j < data->numOuts; j++){
            temp = data->tree->output(j);
            *out++ = temp;
        }
        *writePos++ = temp;
    }
    return 0;
}



int main(void)
{
    PaError err;
    paData data;
    data.numOuts = 2;
    data.tree = root_ptr_f(new root_f(1, data.numOuts));
    char filename[16];
    
#if ( CIRCUIT == 1)
    sprintf(filename, "circuit1.wav");
    //declare constants
    float carrAmp = 0.5;
    float carrFreq = 100;
    float modAmp = 400;
    float modFreq =  100;
    
    //create ugens
    node_ptr_f modulator = data.tree->addNode(node_ptr_f(new sinosc_f("modulator", 2, 1, sr, modFreq, modAmp)));
    node_ptr_f adder = data.tree->addNode(node_ptr_f(new add_f("adder", 2, 1)));
    node_ptr_f carrier = data.tree->addNode(node_ptr_f(new sinosc_f("carrier", 2, 1, sr, carrFreq, carrAmp)));
    
    //connect ugens
    modulator->setInput(SINOSC_FREQ, modFreq);
    modulator->setInput(SINOSC_AMP, modAmp);
    
    adder->setInput(0, carrFreq);
    adder->setInput(1, 0, modulator);
    
    carrier->setInput(SINOSC_FREQ, 0, adder);
    carrier->setInput(SINOSC_AMP, carrAmp);
    
    data.tree->setInput(0, 0, carrier);
    
#elif (CIRCUIT == 2)
    sprintf(filename, "circuit2.wav");
    //declare constants
    float modIndex = 0;
    float modIndexDev = 1;
    float carrAmp = 0.5;
    float carrFreq = 440;
    float modFreq =  220;
    
    //create ugens
    node_ptr_f carrenv = data.tree->addNode(node_ptr_f(new expenv_f("carrenv", 0, 1, sr, carrAmp,
                                                             0.35 * dur_sec, 0.05 * dur_sec, 0.2 * dur_sec, 0.3 * dur_sec,
                                                             0.7 * carrAmp, 2)));
    node_ptr_f modenv = data.tree->addNode(node_ptr_f(new expenv_f("modenv", 0, 1, sr, modIndexDev,
                                                            0.05 * dur_sec, 0.2 * dur_sec, 0.2 * dur_sec, 0.55 * dur_sec,
                                                            0.5 * modIndexDev, 3)));
    node_ptr_f adder1 = data.tree->addNode(node_ptr_f(new add_f("adder1", 2, 1)));
    node_ptr_f mult = data.tree->addNode(node_ptr_f(new mult_f("mult", 2, 1)));
    node_ptr_f modulator = data.tree->addNode(node_ptr_f(new sinosc_f("modulator", 2, 1, sr, 1, 1)));
    node_ptr_f adder2 = data.tree->addNode(node_ptr_f(new add_f("adder2", 2, 1)));

    node_ptr_f carrier = data.tree->addNode(node_ptr_f(new sinosc_f("carrier", 2, 1, sr, carrFreq, 1.0)));
    
    //connect ugens
    adder1->setInput(0, modIndex);
    adder1->setInput(1, 0, modenv);
    
    mult->setInput(0, modFreq);
    mult->setInput(1, 0, adder1);
    
    modulator->setInput(SINOSC_FREQ, modFreq);
    modulator->setInput(SINOSC_AMP, 0, mult);
    
    adder2->setInput(0, carrFreq);
    adder2->setInput(1, 0, modulator);
    
    carrier->setInput(SINOSC_FREQ, 0, adder2);
    carrier->setInput(SINOSC_AMP, 0, carrenv);
    
    data.tree->setInput(0, 0, carrier);

#elif (CIRCUIT == 3)
    sprintf(filename, "circuit3.wav");
    //declare constants
    float modIndex = 0;
    float modIndexDev = 1;
    float carrAmp = 0.5;
    float carrFreq1 = 300;
    float carrFreq2 = 2100;
    float modFreq =  300;
    
    //create ugens
    node_ptr_f carrenv = data.tree->addNode(node_ptr_f(new expenv_f("carrenv", 1, 2, sr, carrAmp,
                                                            0.35 * dur_sec, 0.05 * dur_sec, 0.2 * dur_sec, 0.3 * dur_sec,
                                                            0.7 * carrAmp, 2)));
    node_ptr_f modenv = data.tree->addNode(node_ptr_f(new expenv_f("modenv", 1, 1, sr, modIndexDev,
                                                           0.05 * dur_sec, 0.2 * dur_sec, 0.2 * dur_sec, 0.55 * dur_sec,
                                                           0.5 * modIndexDev, 3)));
    node_ptr_f adder1 = data.tree->addNode(node_ptr_f(new add_f("adder1", 2, 1)));
    node_ptr_f modulator = data.tree->addNode(node_ptr_f(new sinosc_f("modulator", 2, 2, sr, 1, 1)));
    
    node_ptr_f mult1 = data.tree->addNode(node_ptr_f(new mult_f("mult1", 2, 1)));
    node_ptr_f adder2 = data.tree->addNode(node_ptr_f(new add_f("adder2", 2, 1)));
    node_ptr_f adder3 = data.tree->addNode(node_ptr_f(new add_f("adder3", 2, 1)));

    node_ptr_f mult2 = data.tree->addNode(node_ptr_f(new mult_f("mult2", 2, 1)));
    node_ptr_f mult3 = data.tree->addNode(node_ptr_f(new mult_f("mult3", 2, 1)));
    node_ptr_f carrier1 = data.tree->addNode(node_ptr_f(new sinosc_f("carrier1", 2, 1, sr, carrFreq1, 1.0)));
    node_ptr_f carrier2 = data.tree->addNode(node_ptr_f(new sinosc_f("carrier2", 2, 1, sr, carrFreq2, 1.0)));

    node_ptr_f adder4 = data.tree->addNode(node_ptr_f(new add_f("adder4", 2, 1)));
    
    //connect ugens
    adder1->setInput(0, modIndex);
    adder1->setInput(1, 0, modenv);
    
    mult1->setInput(0, modFreq);
    mult1->setInput(1, 0, adder1);
    
    modulator->setInput(SINOSC_FREQ, modFreq);
    modulator->setInput(SINOSC_AMP, 0, mult1);
    
    adder2->setInput(0, 80.0);
    adder2->setInput(1, 0, modulator);
    
    mult2->setInput(0, 1, modulator);
    mult2->setInput(1, 0.5);
    
    adder3->setInput(0, 0, mult2);
    adder3->setInput(1, carrFreq2);
    
    carrier1->setInput(SINOSC_FREQ, 0, adder2);
    carrier1->setInput(SINOSC_AMP, 0, carrenv);
    
    mult3->setInput(0, 1, carrenv);
    mult3->setInput(1, 0.2);
    
    carrier2->setInput(SINOSC_FREQ, 0, adder3);
    carrier2->setInput(SINOSC_AMP, 0, mult3);
    
    adder4->setInput(0, 0, carrier1);
    adder4->setInput(1, 0, carrier2);
    
    data.tree->setInput(0, 0, adder4);
#endif
    
    SF_INFO sfinfo;
    sfinfo.channels = 1;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    sfinfo.samplerate = sr;
    sf = sf_open(filename, SFM_WRITE, &sfinfo);
    if (!sf) {
        printf("Error opening file!\n");
        return -2;
    }
    
    err = Pa_Initialize();
    if( err != paNoError )
        printf( "PortAudio error: %s\n", Pa_GetErrorText( err ) );
    
    PaStream *stream;
    err = Pa_OpenDefaultStream( &stream,
                               0, /* no input channels */
                               2, /* stereo output */
                               paFloat32, /* 32 bit floating point output */
                               sr,
                               BLOCK_SIZE, /* frames per buffer */
                               paCallback, /* this is your callback function */
                               &data ); /*This is a pointer that will be passed to
                                         your callback*/
    if( err != paNoError )
        printf( "PortAudio error: %s\n", Pa_GetErrorText( err ) );
    
    err = Pa_StartStream( stream );
    if( err != paNoError )
        printf( "PortAudio error: %s\n", Pa_GetErrorText( err ) );
    
    
    //need some extra time for the callback to finish
    Pa_Sleep(dur_sec * 1000 + (2 * BLOCK_SIZE/sr));
    
    err = Pa_StopStream( stream );
    if( err != paNoError )
        printf( "PortAudio error: %s\n", Pa_GetErrorText( err ) );
    
    err = Pa_CloseStream(stream);
    if( err != paNoError )
        printf( "PortAudio error: %s\n", Pa_GetErrorText( err ) );
    
    err = Pa_Terminate();
    if( err != paNoError )
        printf( "PortAudio error: %s\n", Pa_GetErrorText( err ) );
  
    sf_writef_float(sf, sfBuffer, sfSamples);

    sf_close(sf);

    printf("Hello World!\n");
    return 0;
}
