
#ifndef PHYSICALMODEL_H_INCLUDED
#define PHYSICALMODEL_H_INCLUDED
#include <iostream>
#include <vector>
#include <numeric>

std::vector<double> gl_RC;
std::vector<double> gl_spatial;
std::vector<double> gl_M1;
std::vector<double> gl_M2;
std::vector<double> gl_M3;

std::vector<double> gl_mem1;
std::vector<double> gl_mem2;
std::vector<double> gl_mem3;


////////////////////////////////////
// Mebrane class
class Membrane
{
public:
    Membrane(float _Lx,float _Ly,float _T,float _sigma0,float _sigma1_m,float _E,float _H,float _volDens,int _Modes,float _gamma)
    :
    Lx(_Lx), Ly(_Ly), T(_T), sigma0(_sigma0), sigma1_m(_sigma1_m), E(_E), H(_H), volDens(_volDens), Modes(_Modes), gamma(_gamma)
    {
        
        int strikeDurationInSamples = strikeDuration * m_defaultSampleRate;

        
        spatial.resize(Modes*Modes); gl_spatial.resize(Modes*Modes);
        M1.resize(Modes*Modes); M2.resize(Modes*Modes); M3.resize(Modes*Modes);
        gl_M1.resize(Modes*Modes); gl_M2.resize(Modes*Modes); gl_M3.resize(Modes*Modes);
        mem1.resize(Modes*Modes); mem2.resize(Modes*Modes); mem3.resize(Modes*Modes);
        gl_mem1.resize(Modes*Modes); gl_mem2.resize(Modes*Modes); gl_mem3.resize(Modes*Modes);
        MM1.resize(Modes*Modes); MM2.resize(Modes*Modes); MM3.resize(Modes*Modes);
        RC.resize(strikeDurationInSamples); gl_RC.resize(strikeDurationInSamples);
        
        //Am  = (double*)calloc(Modes*Modes,sizeof(double));
        modalVector.resize(Modes*Modes);
        
        for (int i=0; i < strikeDurationInSamples; i++)
        {
            RC[i] = 5 * (1-cos((float_Pi/200) * ( double(i) / m_defaultSampleRate) )); // Raised cosine excitation signal
        }

        
        float x_0 = 0.2; float y_0 = 0.7, nu = 0.3, D = (H*H*H*E) / ((12*(1-nu*nu))) , mu = (H * volDens);
        float k = 1.f / m_defaultSampleRate;
 
        verticalModes.resize(Modes * Modes);
        horizontalModes.resize(Modes * Modes);
        
        for (int i=0; i < Modes; i++)
        {
            for (int j=0; j < Modes; j++)
            {
                verticalModes[i*Modes+j] = i+1;
                horizontalModes[i*Modes+j] = j+1;
            }
        }
        
        for (int j = 0; j < Modes*Modes; j++)
        {
            mem1[j] = 0; mem2[j] = 0; mem3[j] = 0;
            
            modalVector[j]  = pow( (float_Pi * verticalModes[j]) / Lx ,2) + pow((float_Pi * horizontalModes[j]) / Ly ,2) ;
            
            spatial[j] = sin(float_Pi * static_cast<double>(verticalModes[j])*(x_0/Lx)) * sin(float_Pi * static_cast<double>(horizontalModes[j])*(y_0/Ly));
            
            // Current displacement
            M1[j] = (2.0 - double(5.1419e-10) * double( ( T * modalVector[j] + D * modalVector[j] * modalVector[j] ) / mu)) / (1 + double(k*(sigma0 + sigma1_m * modalVector[j])));
            
            // Previous displacement
            M2[j] = double((-1+ k*(sigma0 + sigma1_m * modalVector[j] ) ) / (1 + k*(sigma0 + sigma1_m * modalVector[j])));
            
            // Current forcing
            M3[j] = double(( ((4 * double(5.1419e-10))/(mu*Lx*Ly)) * sin(float_Pi * verticalModes[j] * (x_0/Lx)) * sin(float_Pi * horizontalModes[j] * (y_0/Ly)) ) / (1+k*(sigma0 + sigma1_m * modalVector[j])));
        }
    }
    
    std::vector<double>& getCurrentDisplacementVector()
    {
        return M1;
    }
    
    
    std::vector<double>& getPreviousDisplacementVector()
    {
        return M2;
    }
    
    
    std::vector<double>& getCurrentForcingVector()
    {
        return M3;
    }
    
    
    std::vector<double>& getExcitationSignal()
    {
        return RC;
    }
    
    
    std::vector<double>& getSpatialComponentVector()
    {
        return spatial;
    }
    
    int getStrikeDuration()
    {
        return strikeDuration * m_defaultSampleRate;
    }
    
    
    std::vector<double> spatial, M1,M2,M3,RC,mem1,mem2,mem3, MM1, MM2,MM3;

    
private :
    const int m_defaultSampleRate { 44100 };
    float Lx,Ly,T,sigma0,sigma1_m,E,H,volDens;
    //double* Am;
    
    int Modes;
    float gamma;
    
    std::vector<double> modalVector;
    std::vector<float> verticalModes;
    std::vector<float> horizontalModes;
    
    const float strikeDuration { 5.f }; //in milliseconds
};


class PercussionRenderer
{
public:
	PercussionRenderer(int m, float vol, float fs, float stiffness,float damp, float thick, int mat, float volume)
    :
    m_midiNoteNumber(m),
    m_youngModulus(stiffness),
    m_damping(damp),
    m_thickness(thick),
    m_material(mat),
    m_volume(volume)
	{
        // Map parameters: Material option will define density and E while note number will define the sizes
        //--- Material density
        float volDens;
        if (m_material == 1)
        {
            volDens = 3050;
        }
        else
        {
            volDens = 10050;
        }

        //--- Young's Modulus <---> Stiffness! (probably the parameter with the greatest effect):
        // Map (0.0,1.0) to physical Y.M. values: (1e09,1e11)
        m_youngModulus = jmap(m_youngModulus, 0.f, 1.f, 1000000000.f, 100000000000.f);
        
        
        //--- Dimensions are specified based on the current MIDI note number.
        // We are assuming a square plate so Lx = Ly!
        float Lx = jmap(static_cast<float>(m_midiNoteNumber), MIN_MIDI_NOTE, MAX_MIDI_NOTE, MIN_LENGTH, MAX_LENGTH);
        float Ly = Lx;

        //--- Calculate other parameters and find "allowable" number of modes
        float D =  (m_thickness * m_thickness * m_thickness * m_youngModulus) / (( 12 * (1- nu * nu)));
        
        float mu = (m_thickness * volDens);
        
        // Two loss terms, proportionate to each other!
        float sigma0 = m_damping, sigma1_m = 0.1f * sigma0;
        
        float GAMMA =  - 4.0 * m_defaultSampleRate * m_defaultSampleRate;
        double temp = double_Pi / Lx;
        
        float ALPHA = static_cast<double>(D/mu) * static_cast<double>(pow(temp, 4)  + (2 * pow(double_Pi, 4))/(pow(Ly, 4) ) + pow(temp, 4) );
        float BETA =  - static_cast<double>(tension / mu) * double(pow(pi/Lx, 2) + pow(pi/Ly, 2));
        float DELTA = BETA  * BETA - 4 * ALPHA * GAMMA;
        
        float gammaMin = 5, gammaMax = 10;
        m_gamma = (gammaMax - gammaMin) * (sigma0) + gammaMin;
        
        //Bind number of modes so that solution doesn't explode!
        m_numModes = abs(static_cast<int> (sqrt((sqrt(DELTA) -  BETA) / (2*ALPHA)))) / 2 ;
        m_numModes = jmin(m_numModes, 1000); // Efficiency Fudginess! For now let's bind our number of modes @ 1000
        
        m_samplesCounter = 0;
        MM1.resize(m_numModes * m_numModes);
        MM2.resize(m_numModes * m_numModes);
        MM3.resize(m_numModes * m_numModes);
   
        membrane = new Membrane(Lx, Ly, tension, sigma0, sigma1_m, m_youngModulus, m_thickness, volDens, m_numModes, m_gamma);
	}

	
    // DSP CODE
	void renderToBuffer(float* outbuf, int samples, bool isDown)
	{
        gl_RC = membrane->RC;
        gl_spatial = membrane->spatial;
        gl_M1 = membrane->M1;
        gl_M2 = membrane->M2;
        gl_M3 = membrane->M3;
        
        gl_mem2 = MM2;
        gl_mem3 = MM3;
        
        int MODES = m_numModes * m_numModes;
        
        int strikeDurationInSamples = membrane->getStrikeDuration();
        
        for (int n = 0; n < samples; n++)
        {
            //~~~~~~~~~~~ For every sample of the buffer, produce a temporal component vector.
            if (m_samplesCounter > 2)
            {
                if (m_samplesCounter < strikeDurationInSamples)
                {
                    //~~~~~~~~~~~ Here, we will include the forcing samples RC.
                    for (int i=0; i < MODES; i++)
                    {
                        gl_mem1[i] = gl_M1[i] * gl_mem2[i] + gl_M2[i]*gl_mem3[i] + gl_M3[i] * gl_RC[m_samplesCounter];
                    }
                }
                else
                {
                    //~~~~~~~~~~~ Here, the forcing is over. Only previous states affect the vibration
                    for (int i=0; i < MODES; i++)
                    {
                        gl_mem1[i] = gl_M1[i] * gl_mem2[i] + gl_M2[i] * gl_mem3[i];
                    }
                }
            }
            

            float gainCoeff = (m_volume / 70.0) * double(exp(- m_gamma * m_samplesCounter / m_defaultSampleRate ));
            
            // The simulation output will most probably be in the order of 10^(-15) and since we can't normalise the output we need
            // to scale it with an experimentally found gain factor
            float normalisingFactor = 3e15;
            outbuf[n] =  gainCoeff * static_cast<double>( normalisingFactor * std::inner_product(begin(gl_spatial), end(gl_spatial), begin(gl_mem1), 0.0));

            gl_mem3 = gl_mem2; gl_mem2 = gl_mem1; // Update the previous displacement vectors
            
            // After buffer has been "recycled", we need a way to keep an index of the previous states, mem1,mem2,mem3
            MM2 = gl_mem2;
            MM3 = gl_mem3;
            
            m_samplesCounter++;
        }
        
    }
    
	bool isAlive()
    {
        return m_samplesCounter < m_defaultDuration;
	}

	int getIndex()
    {
		return m_midiNoteNumber;
	}
    
    
    // Setters of min/max lengths in case we want to change the range of the produced tones
    // based on the incoming MIDI
    void setMinimumLength(float newMinLength)
    {
        MIN_LENGTH = newMinLength;
    }
    
    void setMaximumLength(float newMaxLength)
    {
        MAX_LENGTH = newMaxLength;
    }
    
private:
    
    std::vector<double> spatial, M1,M2,M3,RC,mem1,mem2,mem3 , MM1, MM2, MM3;
    
    int m_numModes, m_samplesCounter, m_midiNoteNumber, m_material;
    
    float m_tension, m_youngModulus,m_damping, m_thickness, m_volume;
    
    const float pi = 3.141592653589793238462643f;
    const int m_defaultDuration { 44100 };
    
    const int m_defaultSampleRate { 44100 };
    
    const float MIN_MIDI_NOTE { 21.f };
    const float MAX_MIDI_NOTE { 108.f };
    
    float MIN_LENGTH { 0.5f };
    float MAX_LENGTH { 1.5f };
    
    float m_gamma; // loss term
    
    const float nu = 0.3; // Poisson constant
    const float tension = 100; // Constant tension: changing this, does not alter result much

    
    ScopedPointer<Membrane> membrane;
};

#endif  // PHYSICALMODEL_H_INCLUDED
