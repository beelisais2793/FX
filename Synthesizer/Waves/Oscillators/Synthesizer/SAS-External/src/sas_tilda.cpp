// include flext header
#include <flext/flext.h>
#include <iostream>

// check for appropriate flext version
#if !defined(FLEXT_VERSION) || (FLEXT_VERSION < 401)
#error You need at least flext version 0.4.1
#endif


#include <fts.h>
#include "sas/sas.h"

#define ENVELOPE_SIZE 20
#define ENVELOPE_BASE (SAS_MAX_AUDIBLE_FREQUENCY / ENVELOPE_SIZE)

struct source_data_s {
  sas_source_t source;
  char * filename;
  sas_file_t file;
  int number_of_frames;
  sas_frame_t frame;
  struct sas_position_s pos;
};

static void
update_callback (sas_synthesizer_t s,
		 sas_source_t source,
		 sas_frame_t * frame,
		 sas_position_t * pos,
		 void * call_data)
{
  struct source_data_s * sd;
  sd = (struct source_data_s *) call_data;
  *frame = sd->frame;
  *pos = &sd->pos;
}

using namespace std;

class sas : public flext_dsp
{
	FLEXT_HEADER(sas,flext_dsp)
 
public:
	sas()
	{ 
		//create sas synth and frame
		m_synth = sas_synthesizer_make ();

		m_amp=0.5;
		m_freq=440;	

		for (int i = 0; i < ENVELOPE_SIZE; i++) {
			m_color[i] = 1.0;
		}
		m_colorEnvelope = sas_envelope_make(ENVELOPE_BASE, ENVELOPE_SIZE, m_color);
		sas_envelope_adjust_for_color (m_colorEnvelope);

		for (int i = 0; i < ENVELOPE_SIZE; i++) {
			m_warp[i]=ENVELOPE_BASE*float(i+1);
			m_warpIdentity[i]=ENVELOPE_BASE*float(i+1);
		}
		m_warpEnvelope = sas_envelope_make(ENVELOPE_BASE, ENVELOPE_SIZE, m_warp);
		sas_envelope_adjust_for_warp (m_warpEnvelope);
		

		m_sourceData.frame = sas_frame_make ();
		sas_frame_set_amplitude (m_sourceData.frame, m_amp);
		sas_frame_set_frequency (m_sourceData.frame, m_freq);
		sas_frame_set_color (m_sourceData.frame, m_colorEnvelope);
		sas_frame_set_warp (m_sourceData.frame, m_warpEnvelope);
		m_sourceData.pos.x = m_sourceData.pos.y = m_sourceData.pos.z = 0.0;
		
		m_sourceData.source = sas_synthesizer_source_make (m_synth, &m_sourceData.pos, update_callback, &m_sourceData);


		sas_synthesizer_synthesize (m_synth, m_outputBuffer);

		m_counter=0;

		AddInSignal("audio in");
		AddInFloat("amplitude");
		AddInFloat("frequency");
		AddInAnything("color (spectral envelope");
		AddInAnything("warping (spectral envelope");
		AddOutSignal("audio out L");
		AddOutSignal("audio out R");

		FLEXT_ADDMETHOD(1, setAmp);
		FLEXT_ADDMETHOD(2, setFreq);
		FLEXT_ADDMETHOD(3, setColor);
		FLEXT_ADDMETHOD(4, setWarping);
		
		post("sas~ : structured additive synthesis \n created with flext \n using libsas by Sylvain Marchand and Anthony Beurive \n at SCRIME, University of Bordeaux");
	}

	~sas()
	{
		sas_synthesizer_free (m_synth);
		sas_frame_free (m_sourceData.frame);
	}

protected:

	virtual void m_signal(int n, float *const *in, float *const *out);

private:

	// FLEXT_CALLBACK
	FLEXT_CALLBACK_F(setAmp)
	void setAmp(float amp)
	{
		m_amp = amp;
	}

	FLEXT_CALLBACK_F(setFreq)
	void setFreq(float freq)
	{
		m_freq = freq;
	}

	FLEXT_CALLBACK_A(setColor)
	void setColor(const t_symbol *s, int argc, t_atom *argv)
	{
		//if we got a list 
		if(s==sym_list) {
			for (int i = 0; i < ENVELOPE_SIZE && i<argc; i++) {
				m_color[i] = double(GetFloat(argv[i]));
				if(m_color[i]<0) {
					m_color[i]=0;
				}
			}
			m_colorEnvelope = sas_envelope_make(ENVELOPE_BASE, ENVELOPE_SIZE, m_color);
			sas_envelope_adjust_for_color (m_colorEnvelope);
		}
		else if(s==sym_bang) {
			for (int i = 0; i < ENVELOPE_SIZE; i++) {
				m_color[i] = 1.0;
			}
			m_colorEnvelope = sas_envelope_make(ENVELOPE_BASE, ENVELOPE_SIZE, m_color);
			sas_envelope_adjust_for_color (m_colorEnvelope);
		}
	}

	FLEXT_CALLBACK_A(setWarping)
	void setWarping(const t_symbol *s,int argc,t_atom *argv)
	{
		//if we got a list 
		if(s==sym_list) {
			for (int i = 0; i < ENVELOPE_SIZE && i<argc; i++) {
				m_warp[i] = double(GetFloat(argv[i]));
				if(m_warp[i]>1.5) {
					m_warp[i]=1.5;
				}
				else if(m_warp[i]<0.5) {
					m_warp[i]=0.5;
				}	
				m_warp[i]*=m_warpIdentity[i];
			}
			m_warpEnvelope = sas_envelope_make(ENVELOPE_BASE, ENVELOPE_SIZE, m_warp);
			sas_envelope_adjust_for_warp (m_warpEnvelope);
		}
		else if(s==sym_bang) {
			for (int i = 0; i < ENVELOPE_SIZE; i++) {
				m_warp[i]=m_warpIdentity[i];
			}
			m_warpEnvelope = sas_envelope_make(ENVELOPE_BASE, ENVELOPE_SIZE, m_warp);
			sas_envelope_adjust_for_warp (m_warpEnvelope);
		}
	}
	
	double m_amp;
	double m_freq;
	double m_color[ENVELOPE_SIZE];
	double m_warp[ENVELOPE_SIZE];
	double m_warpIdentity[ENVELOPE_SIZE];

	source_data_s m_sourceData;
	sas_synthesizer_t m_synth;
	sas_envelope_t m_warpEnvelope;
	sas_envelope_t m_colorEnvelope;
	double m_outputBuffer[2*SAS_SAMPLES];
	int m_counter;
};

// instantiate the class
FLEXT_NEW_DSP("sas~", sas)

void sas::m_signal(int nbFrames, float *const *in, float *const *out)
{
	
	/*
	sas_frame_set_amplitude (sd1.frame, m_amp);
	sas_frame_set_frequency (sd1.frame, m_freq);
	sas_frame_set_color (sd1.frame, e);
	*/
	for(int f=0; f<nbFrames; ++f) {
		/*
		out[0][n]=float(outputBuffer[m_counter]);
		out[1][n]=float(outputBuffer[m_counter+SAS_SAMPLES]);
		m_counter++;
		if(m_counter>=SAS_SAMPLES) {
			sas_synthesizer_synthesize (synth, outputBuffer);
			m_counter=0;
		}
		*/

		out[0][f]=float(m_outputBuffer[m_counter]);
		m_counter++;
		out[1][f]=float(m_outputBuffer[m_counter]);
		m_counter++;

		if(m_counter>=SAS_SAMPLES*2) {
			sas_frame_set_amplitude (m_sourceData.frame, m_amp);
			sas_frame_set_frequency (m_sourceData.frame, m_freq);
			sas_frame_set_color (m_sourceData.frame, m_colorEnvelope);
			sas_frame_set_warp (m_sourceData.frame, m_warpEnvelope);
			sas_synthesizer_synthesize (m_synth, m_outputBuffer);
			m_counter=0;
		}
	}
}



