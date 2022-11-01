//
//  Delay.h
//  String Synthesis (Karplus)
//

#ifndef Delay_h
#define Delay_h

class DelayUnit {
    std::vector<float> m_delayBuffer;
    int m_writePos;
public:
    DelayUnit(int maxDelayTime) :
    m_delayBuffer(maxDelayTime),
    m_writePos(0)
    {
    }
    
    DelayUnit() :
    m_delayBuffer(44100),
    m_writePos(0)
    {
    }
    
    float getMaxDelayTime(void) {
        return m_delayBuffer.size();
    }
    
    void write(float value) {
        while (m_writePos >= getMaxDelayTime())m_writePos -= getMaxDelayTime();
        while (m_writePos < 0)m_writePos += getMaxDelayTime();
        
        m_delayBuffer[m_writePos] = value;
        
        ++m_writePos;
    }
    
    float getDelay(int delayTime) {
        int readPos = m_writePos - delayTime;
        
        while (readPos >= getMaxDelayTime())readPos -= getMaxDelayTime();
        while (readPos < 0)readPos += getMaxDelayTime();
        
        return m_delayBuffer[readPos];
    }
    
    float clip(float value) {
        if (value > 1) value = 1;
        if (value < -1) value = -1;
        return value;
    }
    
};


#endif /* Delay_h */
