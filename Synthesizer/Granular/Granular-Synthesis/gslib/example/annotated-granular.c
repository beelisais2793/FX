//Compiles correctly with MINGW GCC 3.4.2 on Windows
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "grain_engine.h"
#include "allegro.h"  // Allegro [http://www.talula.demon.co.uk/allegro] used for audio i/o and .wav reading

//Output sample rate
#define SAMPLE_RATE 44100

//Output buffer size. Shorter has less latency but can lead to clicking artifacts when the buffer underruns
#define BUFSIZE 4096

static AUDIOSTREAM *out_stream; //Allegro's output audio stream structure

//Wave files to used
static char *wave_names[] = {"sh5.wav", "choir1.wav", "bass2.wav"};
static int n_waves = 3; // Number of wave files


// Load the waveforms
void load_waveforms ()
{
  SAMPLE *samp;
  grain_source *source; // Grain Source structure defines the properties of a grain generator 
  int i;


  for (i = 0; i < n_waves; i++)
    {
      // Read each wave and put it in a grain_source structure
      samp = load_sample (wave_names[i]); // Allegro function
      
      if(samp==NULL)
      {
        printf("Sample %s missing\n", wave_names[i]);
        exit(-1);
      }
      source = create_uniform_source (i, samp->data, samp->len);
      add_source (source); // Register the source with the engine (important!)
    }
}

//Audio generation loop
void play_audio ()
{
  unsigned char *buf, *dbl_buf;
  long rest_time;
  int i; 
  int copied = 0;
  
  // Create a double buffer (16 bit samples so 2 bytes for each sample)
  dbl_buf = malloc(BUFSIZE*2);
  
  //Start the audio stream, 16 bit mono stream at SAMPLE_RATE
  out_stream =
    play_audio_stream (BUFSIZE, 16, FALSE, SAMPLE_RATE, 255, 128);

  //Set the sub-buffer length (grain callback is called for each sub-buffer)
  set_buffer_length (128);
  
  //This following loop can also be implemented in a multi-threaded manner
  
  //Check keyboard state (allegro function), and exit when the user presses escape
  while (key[KEY_ESC] == 0)
    {
          //Get next audio buffer (if one's available)
          buf = get_audio_stream_buffer (out_stream);
          
          //If no buffer available
          if (buf == NULL)
          {
          
            //If new buffer not already created
            if(!copied)
             {
              //Call the synthesis algorithm to fill the buffer
              fill_buffer (dbl_buf, BUFSIZE * 2 );	     
              copied = 1;
            }
            else
              rest(1); // Wait a millisecond
	}
        else // output buffer ready
        {
          // Copy double buffer into output buffer
          memcpy(buf, dbl_buf, BUFSIZE * 2 );
          
          copied = 0;
          //Mark the buffer as ready to play
          free_audio_stream_buffer (out_stream);	  
        }      
    }
    
  //Stop the audio
  stop_audio_stream(out_stream);
}

//This is the callback that updates the properties of a source (i.e. the distribution parameters for that source)
double update_source_pdf (grain_source * source)
{
  static double phase = 0.0;
  double p, pitch_width;  
  
  // Probabilities here are simple sinusiodal functions, phase determined by their ID
  p = fabs(sin(phase+source->id))*0.3; 
  phase+=1e-4; // Update the phase
  
  // Set the probability. THIS MUST BE CALLED on every update_source_pdf
  set_probability (source, p); 
  
  //optional parameters  
  //Set the pitch to a narrow gaussian about its natural rate. Width of the Gaussian changes as a cyclic function  
  //This produces a dissonant sound slowly resolving to clear tones and then back to dissonance
  pitch_width = fabs(cos(phase*0.2+source->id)) * 1e-1;
  set_pitch_distribution(source,1.0,pitch_width, D_GAUSSIAN);
  
  //Set the phase distribution to start at phase*10000, with 500 sample half-Gaussian (negative only) distribution
  //this causes the sounds to "play" forwards very slowly. 
  set_distribution(source, phase*10000, 500, D_GAUSSIAN_BACKWARD); 
  
  //Return the probability this source was assigned
  return p;
}

void init_grains ()
{
  //Set sample rate, bit depth and channels
  set_format (SAMPLE_RATE , 16, 1); 
  
  //Maximum grains (this must be set)
  set_max_grains(200);
  
  //P(generation) = 0.4
  set_grain_density(0.4);
  
  //Set the grain length to 4000 samples (~100ms)
  set_grain_length(4000);  
  
  //Don't normalize the probabilities
  set_normalize(0);  
  
  //Squared Exp. envelope, width 0.1
  set_envelope(ENV_EXP, 0.1);
  
  //Register the callback
  register_distribution_function (update_source_pdf);
  
  //Initialize the engine
  initialize_engine ();

}

int main (int argc, char *argv[])
{
  //Open  the allegro subsystem
  allegro_init ();
  
  //Install the sound driver (if possible)
  if (install_sound (DIGI_AUTODETECT, MIDI_NONE, NULL) != 0)
    {
      printf ("Could not initialize sound driver...\n");
      exit (-1);
    }
  
  //Init RNG
  srand (time (NULL));
  
  //Install keyboard driver.
  install_keyboard ();

  //Initialise the engine (must be donw _BEFORE_ loading the waveforms)
  init_grains ();
  
  //Load the wave data
  load_waveforms ();

  //Start the audio playback loop
  play_audio ();

  //Close down the allegro subsystem
  allegro_exit ();
  return EXIT_SUCCESS;
}
END_OF_MAIN ();