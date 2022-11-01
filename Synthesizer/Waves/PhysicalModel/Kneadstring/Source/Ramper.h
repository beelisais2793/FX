//
//  Ramper.h
//  String Synthesis (Karplus)
//

#ifndef Ramper_h
#define Ramper_h

class Ramp {
    
    enum State {
        Off = 0,
        Attack,
        Decay
    }m_state;
    
    float m_attackDiv, m_decayDiv; //division is the amplitude value increase/decrease per sample
    float m_attackTime, m_decayTime; //attack/decay time in seconds
    float m_sampleRate;
    float m_output; //value that gets outputted in getValue()
    
public:
    
    Ramp(float sr, float attack, float decay) : //pass in these variables when spawn a Ramp
    m_state(Off),
    m_attackTime(attack),
    m_decayTime(decay),
    m_sampleRate(sr)
    {
        calculateDivisions(); //Calculate divisions whenever possible
    }
    
    void setValues(float attack, float decay) {
        m_attackTime = attack;
        m_decayTime = decay;
        
        calculateDivisions();
    }
    
    void setSampleRate(float sampleRate) {
        m_sampleRate = sampleRate;
        
        calculateDivisions();
    }
    
    void calculateDivisions() {
        m_attackDiv = 1.0f/ (m_attackTime * m_sampleRate);
        m_decayDiv = 1.0f / (m_decayTime * m_sampleRate);
    }
    
    //function to start ramp, coincide with midi velocity values e.g. if (vel > 0) trigger()
    void trigger(bool state) {
        if (state == true) {
            m_state = Attack;
        }
        else { m_state = Decay; }
    }
    
    //main function that ramps
    float getValue() {
        if (m_state == Attack) {
            m_output += m_attackDiv;
            if (m_output >= 1.0f) {
                m_state = Decay;
                m_output = 1.0f;
            }
        }
        else if (m_state == Decay) {
            m_output -= m_decayDiv;
            if (m_output <= 0){
                m_state = Off;
            }
        }
        else if (m_state == Off) {
            m_output = 0;
        }
        
        return m_output;
    }
};



#endif /* Ramper_h */
