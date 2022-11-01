#include "RtAudio.h"
#include "kernel.h"
#include <iostream>
#include <cstdlib>
#include "Sine.h"
//Sine *global_sine[100];
#define NUM_SINES 1000
#define _PI 3.1415926535897931
#define NUM_SAMPLES 16
#define SAMPLING_FREQUENCY 44100
//#define SIMPLE 1
// Two-channel sawtooth wave generator.
int saw( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData )
{
  unsigned int i, j;
  double *buffer = (double *) outputBuffer;
  double *lastValues = (double *) userData;
  if ( status )
    std::cout << "Stream underflow detected!" << std::endl;
  // Write interleaved audio data.
  for ( i=0; i<nBufferFrames; i++ ) {
    for ( j=0; j<2; j++ ) {
      *buffer++ = lastValues[j];
      lastValues[j] += 0.005 * (j+1+(j*0.1));
      if ( lastValues[j] >= 1.0 ) lastValues[j] -= 2.0;
    }
  }
  return 0;
}

int additive(void *outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *UserData) {
	float *buffer = (float *)outputBuffer;
	Sine** sine = (Sine **) UserData;
    if ( status ) std::cout << "Stream underflow detected!" << std::endl;
	for (unsigned int i = 0; i < nBufferFrames; i++) {
		float val = 0; 
		for (unsigned int j = 0; j < NUM_SINES; j++) {
			val += sine[j]->tick();
		}
		*buffer++ = val;
	}
	return 0;
}

int additive_sine(void *outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *userData) {
  unsigned int i, j;
  float *buffer = (float *) outputBuffer;
  float *freqs = (float *) userData;
  if ( status )
    std::cout << "Stream underflow detected!" << std::endl;
  // Write interleaved audio data.
  for (i = 0; i < nBufferFrames; i++) {
	  double val = 0;
	  float angle = 2.0f * _PI * i / 44100;
	  for (j = 0; j < NUM_SINES; j++) {
		  val += sin(angle * freqs[j]) / NUM_SINES;
	  }
	  *buffer++ = val;
  }
  return 0;
}

int additive_gpu(void *outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *userData) {
	static float angle = 0;

    if ( status ) std::cout << "Stream underflow detected!" << std::endl;
	
	Additive::compute_sinusoid_gpu_simple((float*)outputBuffer, angle);
	angle+= 2.0f * _PI * NUM_SAMPLES / 44100.f;
	angle = fmod(angle, 44100.f);
	return 0;
}

int additive_gpu_complex(void *outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime,
	RtAudioStreamStatus status, void *userData) {
	static float time = 0;

    if ( status ) std::cout << "Stream underflow detected!" << std::endl;
    Additive::compute_sinusoid_hybrid((float*)outputBuffer, &time);
	time += NUM_SAMPLES / 44100.f;
//	time = fmod(time, 44100.f);
	return 0;
}

#define END_SECS 15
#define MAXSFREQ 3000
#define MAXEFREQ 12000
#define FUNDFREQ 43.654
//developed by jmccarty https://ccrma.stanford.edu/~jmccarty/220/ass5/main.html
void fill_THX(float* freq_start, float* freq_end, float* angle, float *gains, int numSinusoids) {
	float randFreq;
	for (int i=0; i<NUM_SINES; i++){
        //selects random values in range 0->1
        randFreq=(float)(rand()%MAXSFREQ)/(float)MAXSFREQ; 
        //set a starting freqeuncy within range fundamental to max start freq.
        //in order to have more LF components the randnum is squared
		freq_start[i] = randFreq * randFreq*(float)(MAXSFREQ - FUNDFREQ) + FUNDFREQ;
        //generate rand num for end freq.
        randFreq=(float)(rand()%MAXEFREQ)/(float)MAXEFREQ;
        //set end frequencies to ocatve multiples of fundamental
        freq_end[i]=(float)pow(2,(int)(floor(randFreq*9)))*FUNDFREQ;
        //set half of the end freqs to a fifths above fundamental
        if((i % 2) == 0){
			freq_end[i]*=1.5;
		}
    }
        //for 5% of the oscillators chose a random end freq.
        //this adds a little inharmonic content
    for (int i = (int)NUM_SINES*0.05; i>=0; i--){
        randFreq=(float)(rand()%MAXEFREQ)/(float)MAXEFREQ;
        freq_end[i]=(randFreq*200.0+30.0)*FUNDFREQ;
    }
   
    //generate oscillators, lower frequency components have higher amp.
    for(int i=0; i < NUM_SINES;i++)
    {   
		if (i <= (int)NUM_SINES*0.05) {
			gains[i] = 0.3;
			//gains[i] = 0.03;
		}
		else if (freq_end[i] < 2.0*FUNDFREQ) {
			gains[i] = 4.0f;
			//gains[i] = 0.4f;
		}	
		else if (freq_end[i] < 6.0*FUNDFREQ) {
			gains[i] = 2.0f;
			//gains[i] = 0.2f;
		} 	
		else {
			gains[i] = 1.0;
			//gains[i] = 0.1f;
		}

    }
}

int main()
{
  RtAudio dac;
  if ( dac.getDeviceCount() < 1 ) {
    std::cout << "\nNo audio devices found!\n";
    exit( 0 );
  }
  RtAudio::StreamParameters parameters;
  parameters.deviceId = dac.getDefaultOutputDevice();
  parameters.nChannels = 1;
  parameters.firstChannel = 0;
  unsigned int sampleRate = 44100;
  unsigned int bufferFrames = NUM_SAMPLES;// 256 sample frames
  double data[2];
  Sine* sine[NUM_SINES];
  for (int i = 1; i <= NUM_SINES; i++) {
	  Sine* newsine = new Sine();
	  newsine->setSamplingRate(sampleRate);
	  newsine->setFrequency(440 + i* 10);
	  sine[i-1] = newsine;
  }
  float freqs[NUM_SINES];
  for (int i = 1; i <= NUM_SINES; i++) {
	  freqs[i-1] =   440 + i * 10;
  }
 
#if SIMPLE

  Additive::initSynth(NUM_SINES, bufferFrames, freqs);
  try {
    dac.openStream( &parameters, NULL, RTAUDIO_FLOAT32,
                    sampleRate, &bufferFrames, &additive_gpu, (void*)&freqs);
    dac.startStream();
  }
  catch ( RtAudioError& e ) {
    e.printMessage();
    exit( 0 );
  }
#else
  float freqs_start[NUM_SINES];
  float freqs_end[NUM_SINES];
  float angles[NUM_SINES];
  float gains[NUM_SINES];
  //float slideTime = SAMPLING_FREQUENCY * END_SECS* 0.75;
  float slideTime = END_SECS * 0.75;
  fill_THX(freqs_start, freqs_end, angles, gains, NUM_SINES);
  Additive::initSynth_THX(NUM_SINES, bufferFrames, freqs_start, freqs_end, angles, gains, slideTime);
  try {
    dac.openStream( &parameters, NULL, RTAUDIO_FLOAT32,
                    sampleRate, &bufferFrames, &additive_gpu_complex, (void*)&freqs);
    dac.startStream();
  }
  catch ( RtAudioError& e ) {
    e.printMessage();
    exit( 0 );
  }
#endif // 

  
  char input;
  std::cout << "\nPlaying ... press <enter> to quit.\n";
  std::cin.get( input );
  try {
    // Stop the stream
    dac.stopStream();
  }
  catch (RtAudioError& e) {
    e.printMessage();
  }
  if ( dac.isStreamOpen() ) dac.closeStream();
#if SIMPLE
  Additive::endSynth();
#else
  Additive::endSynth_THX();
#endif

  return 0;
}
